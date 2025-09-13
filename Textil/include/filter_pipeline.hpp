/**
 * @file filter_pipeline.hpp
 * @brief Graphics processing pipeline system for applying filters to data buffers
 * 
 * @details This file contains the FilterPipeline class and related utilities for creating
 * complex graphics processing pipelines. The system allows chaining multiple filters
 * together with automatic type checking and buffer management. It supports dynamic
 * buffer allocation, filter composition, and optimized execution paths.
 * 
 * The pipeline system is designed for high-performance graphics processing where
 * multiple effects need to be applied in sequence. It provides type safety through
 * template parameters and runtime validation of filter compatibility.
 * 
 * @note This system requires all filters to be derived from BaseFilter and use
 * FilterableBuffer types for input/output operations.
 */

#ifndef TIL_FILTER_PIPELINE_HPP
#define TIL_FILTER_PIPELINE_HPP

#include "filters.hpp"
#include "numeric_types.hpp"
#include <map>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace til
{
    /**
     * @brief A templated graphics processing pipeline for chaining filters
     * 
     * @details The FilterPipeline class provides a flexible system for creating
     * complex graphics processing chains. It manages the flow of data between
     * filters, automatically allocates intermediate buffers, and ensures type
     * compatibility throughout the pipeline.
     * 
     * The pipeline supports:
     * - Dynamic filter addition and removal
     * - Automatic buffer management
     * - Type-safe filter chaining
     * - Optimized execution paths
     * - Custom buffer assignment
     * 
     * @tparam InputType The type of data entering the pipeline
     * @tparam OutputType The type of data exiting the pipeline
     * 
     * @par Example Usage:
     * @code
     * // Create a pipeline for image processing
     * FilterPipeline<RGBAImage, RGBAImage> pipeline;
     * 
     * // Add filters to the pipeline
     * pipeline.addFilter(&blurFilter)
     *         .addFilter(&sharpenFilter)
     *         .addFilter(&colorAdjustFilter);
     * 
     * // Build the pipeline (validates compatibility)
     * pipeline.build();
     * 
     * // Process data through the pipeline
     * pipeline.run(inputBuffer, outputBuffer, baseData);
     * @endcode
     * 
     * @warning Filters must be compatible (output type of one matches input type of next)
     * @note The pipeline must be rebuilt after adding/removing filters
     */
    template<typename InputType, typename OutputType>
    class FilterPipeline
    {
    public:

        /**
         * @brief Registry for managing typed buffers within the pipeline
         * 
         * @details The BufferRegistry class provides a type-safe mechanism for
         * registering, managing, and retrieving buffers used as intermediate
         * storage between pipeline stages. Each buffer type has its own registry
         * instance to ensure type safety and efficient lookup.
         * 
         * The registry maintains unique IDs for each buffer and provides
         * automatic cleanup when buffers are unregistered. It supports
         * dynamic buffer allocation and deallocation during pipeline execution.
         * 
         * @par Usage:
         * The registry is typically used internally by the FilterPipeline
         * but can be accessed for advanced buffer management scenarios.
         */
        class BufferRegistry
        {
        public:

            /**
             * @brief Register a buffer with the registry
             * 
             * @details Adds a buffer to the registry and assigns it a unique ID.
             * The buffer can then be retrieved using this ID or automatically
             * assigned to pipeline slots based on type compatibility.
             * 
             * @param buffer Pointer to the buffer to register
             * @return Unique ID assigned to the buffer
             * 
             * @throws std::bad_alloc If memory allocation fails
             * 
             * @par Example:
             * @code
             * FilterableBuffer<Color> colorBuffer(width, height);
             * u32 bufferId = registry.registerBuffer(&colorBuffer);
             * @endcode
             */
            u32 registerBuffer(BaseFilterableBuffer *buffer);
            
            /**
             * @brief Remove a buffer from the registry
             * 
             * @details Unregisters a buffer using its ID, making the ID available
             * for reuse. The buffer itself is not destroyed, only removed from
             * the registry's tracking.
             * 
             * @param id The unique ID of the buffer to unregister
             * 
             * @note Does nothing if the ID is not found in the registry
             */
            void unregisterBuffer(u32 id);

            /**
             * @brief Retrieve a buffer by its ID
             * 
             * @details Returns a pointer to the buffer associated with the given ID.
             * This allows direct access to buffers for custom processing or
             * manual buffer assignment.
             * 
             * @param id The unique ID of the buffer to retrieve
             * @return Pointer to the buffer, or nullptr if ID not found
             * 
             * @par Example:
             * @code
             * BaseFilterableBuffer* buffer = registry.getBuffer(bufferId);
             * if (buffer) {
             *     // Use the buffer for processing
             * }
             * @endcode
             */
            BaseFilterableBuffer *getBuffer(u32 id);
            
            /**
             * @brief Get the first available buffer in the registry
             * 
             * @details Returns the first buffer in the registry, useful for
             * automatic buffer assignment when any buffer of the correct
             * type is acceptable. The order is based on internal storage
             * and may not be predictable.
             * 
             * @return Pointer to the first buffer, or nullptr if registry is empty
             */
            BaseFilterableBuffer *getFirstBuffer();

            /**
             * @brief Get the number of buffers in the registry
             * 
             * @details Returns the total count of registered buffers. This can
             * be used to check if buffers are available or to validate registry
             * state before operations.
             * 
             * @return Number of buffers currently registered
             */
            u32 getBufferCount() const;

        private:

            /**
             * @brief Generate the next available unique ID
             * 
             * @details Finds the next unused ID by scanning through used IDs
             * and finding the first gap. This ensures efficient ID reuse
             * while maintaining uniqueness within the registry.
             * 
             * @return Next available unique ID
             */
            u32 getNextId();

        private:

            std::map<u32, BaseFilterableBuffer *> m_buffers;  ///< Map of ID to buffer pointer
            std::set<u32> m_usedIds;                          ///< Set of currently used IDs
        };

        /**
         * @brief Represents a buffer slot in the pipeline
         * 
         * @details A BufferSlot contains metadata about a buffer position in
         * the pipeline, including its expected type and the actual buffer
         * assigned to it. This structure is used internally for pipeline
         * construction and execution.
         * 
         * The type information is used for automatic buffer assignment based
         * on compatibility, while the buffer pointer provides direct access
         * during pipeline execution.
         */
        struct BufferSlot
        {
            std::type_index type = std::type_index(typeid(int));  ///< Expected type for this slot
            BaseFilterableBuffer *buffer = nullptr;              ///< Assigned buffer for this slot
        };

    public:

        /**
         * @brief Add a filter to the end of the pipeline
         * 
         * @details Appends a filter to the pipeline sequence. The filter will
         * be executed after all previously added filters. The pipeline must
         * be rebuilt after adding filters to ensure proper buffer allocation
         * and type compatibility checking.
         * 
         * @param filter Pointer to the filter to add
         * @return Reference to this pipeline for method chaining
         * 
         * @note The pipeline takes ownership of managing the filter during execution
         * @warning The filter must remain valid for the lifetime of the pipeline
         * 
         * @par Example:
         * @code
         * pipeline.addFilter(&blurFilter)
         *         .addFilter(&colorFilter)
         *         .build();
         * @endcode
         */
        FilterPipeline &addFilter(BaseFilter *filter);
        
        /**
         * @brief Insert a filter at a specific position in the pipeline
         * 
         * @details Inserts a filter at the specified index, shifting subsequent
         * filters to later positions. If the index is beyond the current size,
         * the filter is appended to the end.
         * 
         * @param index Position to insert the filter (0-based)
         * @param filter Pointer to the filter to insert
         * @return Reference to this pipeline for method chaining
         * 
         * @note Invalidates the current pipeline build state
         */
        FilterPipeline &insertFilter(u32 index, BaseFilter *filter);
        /**
         * @brief Remove a filter from the pipeline
         * 
         * @details Removes the filter at the specified index from the pipeline.
         * All subsequent filters shift to earlier positions. The pipeline
         * must be rebuilt after removing filters.
         * 
         * @param index Index of the filter to remove (0-based)
         * @return Reference to this pipeline for method chaining
         * 
         * @throws InvalidArgumentError If index is out of range
         * @note Invalidates the current pipeline build state
         */
        FilterPipeline &removeFilter(u32 index);
        
        /**
         * @brief Remove all filters from the pipeline
         * 
         * @details Clears the entire filter sequence, resulting in an empty
         * pipeline. This is useful for resetting the pipeline configuration
         * or preparing for a completely new filter setup.
         * 
         * @return Reference to this pipeline for method chaining
         * @note Invalidates the current pipeline build state
         */
        FilterPipeline &clearFilters();

        /**
         * @brief Add a typed buffer to the pipeline's buffer pool
         * 
         * @details Registers a buffer of the specified type with the pipeline.
         * The buffer can then be automatically assigned to compatible slots
         * during pipeline execution or manually assigned using its ID.
         * 
         * @tparam T The type of data stored in the buffer
         * @param buffer Pointer to the buffer to add
         * @return Unique ID assigned to the buffer
         * 
         * @note The buffer must remain valid for the lifetime of the pipeline
         * @warning Invalidates the current pipeline build state
         * 
         * @par Example:
         * @code
         * FilterableBuffer<Color> tempBuffer(width, height);
         * u32 bufferId = pipeline.addBuffer(&tempBuffer);
         * @endcode
         */
        template<typename T>
        u32 addBuffer(FilterableBuffer<T> *buffer);
        
        /**
         * @brief Remove a typed buffer from the pipeline
         * 
         * @details Removes a buffer of the specified type using its ID. If this
         * was the last buffer of this type, the entire registry for that type
         * is removed to free memory.
         * 
         * @tparam T The type of data stored in the buffer
         * @param id The unique ID of the buffer to remove
         * 
         * @note Invalidates the current pipeline build state
         */
        template<typename T>
        void removeBuffer(u32 id);

        /**
         * @brief Build and validate the pipeline configuration
         * 
         * @details Constructs the internal pipeline structure, validates filter
         * compatibility, creates buffer slots, and prepares the pipeline for
         * execution. This must be called after any configuration changes
         * (adding/removing filters or buffers).
         * 
         * The build process:
         * 1. Checks that filter input/output types are compatible
         * 2. Creates buffer slots for intermediate data
         * 3. Assigns buffers to slots based on type compatibility
         * 4. Validates that all required buffers are available
         * 
         * @throws LogicError If filter types are incompatible
         * @throws LogicError If required buffers are not available
         * 
         * @par Example:
         * @code
         * pipeline.addFilter(&filter1)
         *         .addFilter(&filter2)
         *         .build();  // Must call before run()
         * @endcode
         */
        void build();

        /**
         * @brief Manually assign a buffer to a specific pipeline slot
         * 
         * @details Overrides automatic buffer assignment for a specific slot
         * with a manually chosen buffer. This is useful for custom buffer
         * management or when specific buffers need to be used at specific
         * pipeline stages.
         * 
         * @param slotIndex Index of the buffer slot (0-based)
         * @param bufferId ID of the buffer to assign to the slot
         * 
         * @throws InvalidArgumentError If no buffer is available for the slot type
         * @note Should be called after build() but before run()
         */
        void assignBufferToSlot(u32 slotIndex, u32 bufferId);

        /**
         * @brief Execute the complete pipeline on input data
         * 
         * @details Runs all filters in the pipeline sequence, passing data through
         * intermediate buffers as needed. The pipeline automatically manages
         * buffer flow and ensures proper data transformation from input to output.
         * 
         * The execution process:
         * 1. Validates that the pipeline is built
         * 2. Calls beforePipelineRun() on all filters
         * 3. Executes filters in sequence with appropriate buffers
         * 4. Calls afterPipelineRun() on all filters
         * 
         * @param inputBuffer Buffer containing the input data
         * @param outputBuffer Buffer to receive the processed output
         * @param baseData Base data context passed to all filters
         * 
         * @throws LogicError If pipeline is not built
         * @throws LogicError If buffer assignment fails during execution
         * 
         * @par Performance Notes:
         * - Single filter pipelines bypass intermediate buffering
         * - Buffer reuse is optimized to minimize memory allocations
         * - Filter execution is sequential (no parallelization)
         * 
         * @par Example:
         * @code
         * filters::BaseData context;
         * context.deltaTime = 0.016f;
         * 
         * pipeline.run(inputTexture, outputTexture, context);
         * @endcode
         */
        void run(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer, const filters::BaseData &baseData);

    private:

        /**
         * @brief Validate that all filters in the pipeline have compatible types
         * 
         * @details Checks that the output type of each filter matches the input
         * type of the next filter in the sequence. Also validates that the
         * first filter accepts the pipeline's InputType and the last filter
         * produces the pipeline's OutputType.
         * 
         * @throws LogicError If any filter type incompatibility is found
         */
        void checkFilterCompatibility();
        
        /**
         * @brief Create buffer slots for intermediate data storage
         * 
         * @details Analyzes the filter chain and creates appropriate buffer
         * slots for storing intermediate results between filter stages. Each
         * slot is typed according to the output type of the preceding filter.
         */
        void createBufferSlots();
        
        /**
         * @brief Assign available buffers to created slots
         * 
         * @details Attempts to fill all buffer slots with compatible buffers
         * from the registered buffer pools. Uses type matching to ensure
         * proper buffer assignment.
         * 
         * @throws LogicError If required buffers are not available
         */
        void fillBufferSlots();

    private:

        bool built = true;                                                    ///< Whether the pipeline is built and ready for execution

        std::vector<BaseFilter *> m_filters;                                  ///< Sequence of filters in the pipeline
        std::vector<BufferSlot> m_buffers;                                    ///< Buffer slots for intermediate data
        std::unordered_map<std::type_index, BufferRegistry> m_bufferRegistries; ///< Type-specific buffer registries
    };

    template<typename InputType, typename OutputType>
    u32 FilterPipeline<InputType, OutputType>::BufferRegistry::registerBuffer(BaseFilterableBuffer *buffer) {
        u32 id = getNextId();
        m_buffers[id] = buffer;
        return id;
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::BufferRegistry::unregisterBuffer(u32 id) {
        m_buffers.erase(id);
        m_usedIds.erase(id);
    }

    template<typename InputType, typename OutputType>
    BaseFilterableBuffer *FilterPipeline<InputType, OutputType>::BufferRegistry::getBuffer(u32 id) {
        auto it = m_buffers.find(id);
        return (it != m_buffers.end()) ? it->second : nullptr;
    }

    template<typename InputType, typename OutputType>
    BaseFilterableBuffer *FilterPipeline<InputType, OutputType>::BufferRegistry::getFirstBuffer() {
        if (!m_buffers.empty()) {
            return m_buffers.begin()->second;
        }
        return nullptr;
    }

    template<typename InputType, typename OutputType>
    u32 FilterPipeline<InputType, OutputType>::BufferRegistry::getBufferCount() const {
        return static_cast<u32>(m_buffers.size());
    }

    template<typename InputType, typename OutputType>
    u32 FilterPipeline<InputType, OutputType>::BufferRegistry::getNextId() {
        static u32 id = 1;
        for (u32 usedId : m_usedIds) {
            if (id < usedId) {
                break;
            }
            ++id;
        }
        return id;
    }

    template<typename InputType, typename OutputType>
    FilterPipeline<InputType, OutputType> &FilterPipeline<InputType, OutputType>::addFilter(BaseFilter *filter) {
        m_filters.push_back(filter);
        built = false;
        return *this;
    }

    template<typename InputType, typename OutputType>
    FilterPipeline<InputType, OutputType> &FilterPipeline<InputType, OutputType>::insertFilter(u32 index, BaseFilter *filter) {
        if (index < m_filters.size()) {
            m_filters.insert(m_filters.begin() + index, filter);
        } else {
            m_filters.push_back(filter);
        }
        built = false;
        return *this;
    }

    template<typename InputType, typename OutputType>
    FilterPipeline<InputType, OutputType> &FilterPipeline<InputType, OutputType>::removeFilter(u32 index) {
        if (index >= static_cast<u32>(m_filters.size())) {
            invokeError<InvalidArgumentError>("Filter index out of range");
        }
        m_filters.erase(m_filters.begin() + index);
        built = false;
        return *this;
    }

    template<typename InputType, typename OutputType>
    FilterPipeline<InputType, OutputType> &FilterPipeline<InputType, OutputType>::clearFilters() {
        m_filters.clear();
        built = false;
        return *this;
    }

    template<typename InputType, typename OutputType>
    template<typename T>
    u32 FilterPipeline<InputType, OutputType>::addBuffer(FilterableBuffer<T> *buffer) {
        return m_bufferRegistries[std::type_index(typeid(T))].registerBuffer(buffer);
    }

    template<typename InputType, typename OutputType>
    template<typename T>
    void FilterPipeline<InputType, OutputType>::removeBuffer(u32 id) {
        m_bufferRegistries[std::type_index(typeid(T))].unregisterBuffer(id);
        if (m_bufferRegistries[std::type_index(typeid(T))].getBufferCount() == 0) {
            m_bufferRegistries.erase(std::type_index(typeid(T)));
        }
        built = false;
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::build() {
        if (m_filters.size() != 0) {
            checkFilterCompatibility();
            createBufferSlots();
            
            if (m_filters.size() > 1) {
                fillBufferSlots();
            }
        }

        built = true;
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::assignBufferToSlot(u32 slotIndex, u32 bufferId) {
        auto it = m_bufferRegistries.find(m_buffers[slotIndex].type);
        if (it != m_bufferRegistries.end()) {
            m_buffers[slotIndex].buffer = it->second.getBuffer(bufferId);
        } else {
            invokeError<InvalidArgumentError>("No buffer available for slot");
        }
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::run(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer, const filters::BaseData &baseData) {
        if (!built) {
            invokeError<LogicError>("Pipeline is not built");
        }

        if (m_filters.size() == 0) {
            return;
        }

        for (const auto &filter : m_filters) {
            filter->setBaseData(baseData);
            filter->beforePipelineRun();
        }

        if (m_filters.size() == 1) {
            m_filters[0]->apply(inputBuffer, outputBuffer);
            return;
        }

        BaseFilterableBuffer *currentInput = inputBuffer;
        BaseFilterableBuffer *currentOutput;

        for (int i = 0; i < m_filters.size() - 1; ++i) {
            currentOutput = m_buffers[i].buffer;

            if (!currentOutput) {
                if (m_buffers[i].type == std::type_index(typeid(InputType))) {
                    currentOutput = inputBuffer;
                } else if (m_buffers[i].type == std::type_index(typeid(OutputType))) {
                    currentOutput = outputBuffer;
                } else {
                    invokeError<LogicError>("Could not find a matching buffer");
                }
            }

            m_filters[i]->apply(currentInput, currentOutput);
            currentInput = currentOutput;
        }

        m_filters[m_filters.size() - 1]->apply(currentInput, outputBuffer);

        for (const auto &filter : m_filters) {
            filter->afterPipelineRun();
        }
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::checkFilterCompatibility() {
        std::type_index expectedInput = std::type_index(typeid(InputType));
        for (const auto &filter : m_filters) {
            std::type_index actualInput = filter->inputType;
            if (actualInput != expectedInput) {
                invokeError<LogicError>("Incompatible filter types");
            }
            expectedInput = filter->outputType;
        }
        if (expectedInput != std::type_index(typeid(OutputType))) {
            invokeError<LogicError>("Incompatible filter types");
        }
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::createBufferSlots() {
        m_buffers.clear();
        BufferSlot slot;

        for (int i = 0; i < m_filters.size() - 1; ++i) {
            slot.type = m_filters[i]->outputType;
            slot.buffer = nullptr;
            m_buffers.push_back(slot);
        }
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::fillBufferSlots() {
        for (auto &slot : m_buffers) {
            auto it = m_bufferRegistries.find(slot.type);
            if (it != m_bufferRegistries.end()) {
                slot.buffer = it->second.getFirstBuffer();
            } else if (
                slot.type != std::type_index(typeid(InputType)) &&
                slot.type != std::type_index(typeid(OutputType))
            ) {
                invokeError<LogicError>("No buffer available for slot");
            }
        }
    }
}

#endif // TIL_FILTER_PIPELINE_HPP