/**
 * @file filters.hpp
 * @brief Graphics filter system for data processing and visual effects
 * 
 * @details This file contains the complete filter system for Textil, providing
 * a flexible framework for processing graphics data through customizable filter
 * chains. The system supports both single-pass and multi-threaded processing
 * with type-safe buffer management.
 * 
 * The filter system consists of:
 * - Base classes for filter implementation (BaseFilter, Filter)
 * - Buffer management for typed data storage (FilterableBuffer)
 * - Predefined filters for common graphics operations
 * - Execution modes for performance optimization
 * 
 * Filters can process any data type and support complex transformation
 * pipelines with automatic type checking and buffer management. The system
 * is designed for high-performance graphics processing in terminal environments.
 * 
 * @par Key Features:
 * - Type-safe filter chaining
 * - Multi-threaded processing support
 * - Extensible filter architecture
 * - Built-in graphics filters
 * - Memory-efficient buffer management
 */

#ifndef TIL_FILTERS_HPP
#define TIL_FILTERS_HPP

#include "vector2.hpp"
#include <typeinfo>
#include <typeindex>
#include <future>
#include <thread>
#include <vector>
#include "color.hpp"
#include "character_cell.hpp"
#include <random>
#include "texture.hpp"

namespace til
{
    namespace filters
    {
        /**
         * @brief Base data structure for filter context information
         * 
         * @details Provides common data that all filters can access during
         * processing. This includes timing information and state flags that
         * help filters make context-aware decisions.
         * 
         * Custom filter data structures should inherit from BaseData to
         * maintain compatibility with the filter pipeline system.
         */
        struct BaseData
        {
            f32 time = 0.f;                ///< Current time in seconds for time-based effects
            bool buffer_resized = false;   ///< Flag indicating if buffers were resized this frame
        };
    }

    /**
     * @brief Base class for all filterable buffer types
     * 
     * @details Provides a common interface for buffers that can be processed
     * by filters. This abstraction allows the filter system to work with
     * different data types while maintaining type safety through the derived
     * template classes.
     * 
     * The base class defines common operations like sizing and clearing
     * that are needed by the filter pipeline management system.
     */
    class BaseFilterableBuffer
    {
    public:

        /**
         * @brief Virtual destructor for proper inheritance
         * 
         * @details Ensures proper cleanup when buffers are destroyed
         * through base class pointers in polymorphic scenarios.
         */
        virtual ~BaseFilterableBuffer() = default;

        /**
         * @brief Get the number of elements in the buffer
         * 
         * @details Returns the current size of the buffer in terms of
         * element count. This is used by filters to determine processing
         * boundaries and by the pipeline for buffer management.
         * 
         * @return Number of elements currently in the buffer
         */
        virtual u32 getSize() const;
        
        /**
         * @brief Resize the buffer to contain the specified number of elements
         * 
         * @details Changes the buffer capacity to hold the requested number
         * of elements. May involve memory allocation or deallocation depending
         * on the current buffer state.
         * 
         * @param size New number of elements for the buffer
         * 
         * @note Implementation depends on derived class
         */
        virtual void setSize(u32 size);
        
        /**
         * @brief Clear all elements from the buffer
         * 
         * @details Removes all elements and frees associated memory.
         * After clearing, the buffer size will be zero.
         * 
         * @note Implementation depends on derived class
         */
        virtual void clear();
    };

    /**
     * @brief Typed buffer for storing filterable data elements
     * 
     * @details A template class that provides type-safe storage for data
     * that will be processed by filters. Each buffer stores elements of
     * a specific type and provides both indexed access and bulk operations.
     * 
     * FilterableBuffer is the primary storage mechanism used by the filter
     * system and provides efficient access patterns for both sequential
     * and parallel processing.
     * 
     * @tparam T The type of elements stored in this buffer
     * 
     * @par Thread Safety:
     * FilterableBuffer is not inherently thread-safe. When using concurrent
     * execution modes, ensure proper synchronization or use separate buffer
     * regions for each thread.
     * 
     * @par Example Usage:
     * @code
     * FilterableBuffer<Color> colorBuffer;
     * colorBuffer.setSize(1920 * 1080);
     * 
     * // Fill with data
     * for (u32 i = 0; i < colorBuffer.getSize(); ++i) {
     *     colorBuffer[i] = Color::Red;
     * }
     * 
     * // Process with filter
     * myFilter.apply(&colorBuffer, &outputBuffer);
     * @endcode
     */
    template<typename T>
    class FilterableBuffer : public BaseFilterableBuffer
    {
    public:

        /**
         * @brief Get the number of elements in the buffer
         * 
         * @details Returns the current size of the typed buffer.
         * This provides type-safe access to the buffer size.
         * 
         * @return Number of elements currently in the buffer
         */
        virtual u32 getSize() const override;

        /**
         * @brief Resize the buffer to contain the specified number of elements
         * 
         * @details Changes the buffer to hold the requested number of typed
         * elements. If the new size is larger, new elements are default-constructed.
         * If smaller, excess elements are destroyed.
         * 
         * @param size New number of elements for the buffer
         * 
         * @throws std::bad_alloc If memory allocation fails
         */
        virtual void setSize(u32 size) override;

        /**
         * @brief Clear all elements from the buffer
         * 
         * @details Removes all elements and deallocates storage.
         * After clearing, getSize() will return 0.
         */
        virtual void clear() override;

        /**
         * @brief Get direct access to the underlying storage
         * 
         * @details Returns a reference to the internal vector for
         * cases where direct bulk operations are needed. Use with
         * caution as this bypasses the buffer interface.
         * 
         * @return Reference to the internal storage vector
         * 
         * @warning Direct vector access bypasses buffer abstractions
         */
        std::vector<T> &getBuffer();

        /**
         * @brief Access an element by index
         * 
         * @details Provides mutable access to the element at the
         * specified index. No bounds checking is performed for
         * performance reasons.
         * 
         * @param index Index of the element to access
         * @return Reference to the element at the specified index
         * 
         * @warning No bounds checking - accessing invalid indices causes undefined behavior
         */
        T &operator[](u32 index);
        
        /**
         * @brief Access an element by index (const version)
         * 
         * @details Provides read-only access to the element at the
         * specified index. No bounds checking is performed.
         * 
         * @param index Index of the element to access
         * @return Const reference to the element at the specified index
         * 
         * @warning No bounds checking - accessing invalid indices causes undefined behavior
         */
        const T &operator[](u32 index) const;

    private:

        std::vector<T> m_buffer;  ///< Internal storage for typed elements
    };

    /**
     * @brief Abstract base class for all filter implementations
     * 
     * @details Defines the interface that all filters must implement to
     * participate in the filter pipeline system. Provides execution mode
     * control and type information for pipeline validation.
     * 
     * BaseFilter handles the polymorphic aspects of the filter system,
     * allowing different filter types to be managed uniformly by pipelines
     * while maintaining type safety through runtime checking.
     * 
     * @par Execution Modes:
     * Filters can specify how they should be executed:
     * - Single: Process entire buffers at once
     * - Sequential: Process elements one by one in sequence
     * - Concurrent: Process elements in parallel using multiple threads
     * 
     * @note Derived classes must implement all pure virtual methods
     */
    class BaseFilter
    {
    public:

        /**
         * @brief Execution strategies for filter processing
         * 
         * @details Defines how the filter should process data:
         * - Single: Best for filters that need to process entire datasets
         * - Sequential: Best for simple per-element operations
         * - Concurrent: Best for independent per-element operations that can benefit from parallelism
         */
        enum class ExecutionMode
        {
            Single,      ///< Process entire buffer at once
            Sequential,  ///< Process elements one by one in sequence
            Concurrent   ///< Process elements in parallel using multiple threads
        };

    public:

        ExecutionMode executionMode = ExecutionMode::Single;  ///< How this filter should be executed

        std::type_index inputType = std::type_index(typeid(int));   ///< Expected input data type
        std::type_index outputType = std::type_index(typeid(int));  ///< Produced output data type

    public:

        /**
         * @brief Virtual destructor for proper inheritance
         * 
         * @details Ensures proper cleanup when filters are destroyed
         * through base class pointers in polymorphic scenarios.
         */
        virtual ~BaseFilter() = default;
        
        /**
         * @brief Called before pipeline execution begins
         * 
         * @details Override this method to perform any setup required
         * before the filter processes data. This is called once per
         * pipeline run, regardless of execution mode.
         * 
         * Common uses:
         * - Initialize temporary variables
         * - Prepare lookup tables
         * - Reset state for time-based effects
         */
        virtual void beforePipelineRun() = 0;
        
        /**
         * @brief Apply the filter to input data, producing output data
         * 
         * @details The core filter operation that transforms input data
         * into output data. This method handles the polymorphic dispatch
         * to the appropriate typed filter implementation.
         * 
         * @param input Buffer containing input data to process
         * @param output Buffer to receive the processed output data
         * 
         * @note Input and output buffers must be compatible with the filter's types
         * @warning Buffer sizes and types are not validated at runtime
         */
        virtual void apply(BaseFilterableBuffer *input, BaseFilterableBuffer *output) = 0;
        
        /**
         * @brief Called after pipeline execution completes
         * 
         * @details Override this method to perform any cleanup required
         * after the filter has processed data. This is called once per
         * pipeline run, regardless of execution mode.
         * 
         * Common uses:
         * - Clean up temporary resources
         * - Update statistics or counters
         * - Prepare for next frame
         */
        virtual void afterPipelineRun() = 0;
        
        /**
         * @brief Update the filter's context data
         * 
         * @details Provides the filter with current context information
         * such as timing and state flags. This is called before each
         * pipeline run to ensure filters have current information.
         * 
         * @param baseData Context data containing timing and state information
         */
        virtual void setBaseData(const filters::BaseData &baseData) = 0;
    };

    /**
     * @brief Template base class for typed filter implementations
     * 
     * @details Provides a strongly-typed foundation for implementing filters
     * that process specific data types. Handles the type safety, buffer
     * casting, and execution mode dispatch automatically.
     * 
     * This class bridges the gap between the polymorphic BaseFilter interface
     * and the type-safe filter implementations, providing both flexibility
     * and performance.
     * 
     * @tparam InputType The type of data this filter accepts as input
     * @tparam OutputType The type of data this filter produces as output  
     * @tparam FilterData Custom data structure for filter parameters (inherits from BaseData)
     * 
     * @par Function Types:
     * Filters can be implemented using two different function signatures:
     * - SingleFilterFunction: Processes entire buffers at once
     * - MultiFilterFunction: Processes individual elements
     * 
     * @par Example Implementation:
     * @code
     * struct ColorInvertData : public BaseData {
     *     float intensity = 1.0f;
     * };
     * 
     * class ColorInvert : public Filter<Color, Color, ColorInvertData> {
     * public:
     *     ColorInvert() {
     *         executionMode = ExecutionMode::Concurrent;
     *         setMultiFilterFunction([](Color& input, Color& output, const ColorInvertData& data) {
     *             output.r = 255 - input.r;
     *             output.g = 255 - input.g;
     *             output.b = 255 - input.b;
     *             output.a = input.a;
     *         });
     *     }
     * };
     * @endcode
     */
    template<typename InputType, typename OutputType, typename FilterData = filters::BaseData>
    class Filter : public BaseFilter
    {
    public:

        /**
         * @brief Function signature for filters that process entire buffers
         * 
         * @details This function type is used for filters that need to see
         * the entire dataset to perform their operations, such as FFT-based
         * filters or statistical operations.
         * 
         * @param input Reference to the input buffer
         * @param output Reference to the output buffer  
         * @param data Reference to the filter's configuration data
         */
        using SingleFilterFunction = void (*)(FilterableBuffer<InputType>&, FilterableBuffer<OutputType>&, const FilterData&);
        
        /**
         * @brief Function signature for filters that process individual elements
         * 
         * @details This function type is used for filters that can process
         * each element independently, making them suitable for parallel
         * execution and simpler implementation.
         * 
         * @param input Reference to the input element
         * @param output Reference to the output element
         * @param data Reference to the filter's configuration data
         */
        using MultiFilterFunction = void (*)(InputType&, OutputType&, const FilterData&);

    public:

        FilterData data;  ///< Filter-specific configuration and state data

    public:

        /**
         * @brief Constructor setting up type information
         * 
         * @details Initializes the filter with proper type indices for
         * pipeline validation. Sets up the filter for the specified
         * input and output types.
         */
        Filter();

        /**
         * @brief Virtual destructor for proper inheritance
         * 
         * @details Ensures proper cleanup when filters are destroyed
         * through base class pointers.
         */
        virtual ~Filter() = default;

        /**
         * @brief Called before pipeline execution begins
         * 
         * @details Default implementation does nothing. Override in
         * derived classes if setup is needed before processing.
         */
        virtual void beforePipelineRun() override {}
        
        /**
         * @brief Apply the filter using the configured execution mode
         * 
         * @details Handles type casting and dispatches to the appropriate
         * execution method based on the filter's execution mode. This
         * method is final to ensure consistent behavior.
         * 
         * @param input Polymorphic input buffer
         * @param output Polymorphic output buffer
         */
        virtual void apply(BaseFilterableBuffer *input, BaseFilterableBuffer *output) override final;
        
        /**
         * @brief Called after pipeline execution completes
         * 
         * @details Default implementation does nothing. Override in
         * derived classes if cleanup is needed after processing.
         */
        virtual void afterPipelineRun() override {}
        
        /**
         * @brief Update filter context from base data
         * 
         * @details Copies timing and state information from the base
         * data into the filter's data structure. This method is final
         * to ensure consistent data propagation.
         * 
         * @param baseData Context data from the pipeline
         */
        virtual void setBaseData(const filters::BaseData &baseData) override final;

        /**
         * @brief Set the function for single-buffer processing
         * 
         * @details Assigns a function that will process entire buffers
         * at once. Used when executionMode is Single.
         * 
         * @param func Function to use for single-buffer processing
         */
        void setSingleFilterFunction(SingleFilterFunction func);
        
        /**
         * @brief Set the function for per-element processing
         * 
         * @details Assigns a function that will process individual
         * elements. Used when executionMode is Sequential or Concurrent.
         * 
         * @param func Function to use for per-element processing
         */
        void setMultiFilterFunction(MultiFilterFunction func);

    private:

        /**
         * @brief Cast polymorphic buffers to typed buffers
         * 
         * @details Performs the necessary type casting from base buffer
         * pointers to typed buffer pointers. No type checking is performed
         * for performance reasons.
         * 
         * @param input Base input buffer pointer
         * @param output Base output buffer pointer
         * @return Pair of typed buffer pointers
         * 
         * @warning No runtime type checking is performed
         */
        std::pair<FilterableBuffer<InputType> *, FilterableBuffer<OutputType> *> getBufferPointers(BaseFilterableBuffer *input, BaseFilterableBuffer *output);

        /**
         * @brief Execute filter in single-buffer mode
         * 
         * @details Calls the single filter function if one is assigned.
         * This mode processes the entire buffer at once.
         * 
         * @param inputBuffer Typed input buffer
         * @param outputBuffer Typed output buffer
         */
        void applySingle(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer);
        
        /**
         * @brief Execute filter in sequential mode
         * 
         * @details Processes elements one by one in sequence using the
         * multi filter function. This is the simplest execution mode.
         * 
         * @param inputBuffer Typed input buffer
         * @param outputBuffer Typed output buffer
         */
        void applySequential(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer);
        
        /**
         * @brief Execute filter in concurrent mode
         * 
         * @details Processes elements in parallel using multiple threads.
         * Work is divided evenly among available hardware threads.
         * 
         * @param inputBuffer Typed input buffer
         * @param outputBuffer Typed output buffer
         * 
         * @note Thread count is determined by std::thread::hardware_concurrency()
         */
        void applyConcurrent(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer);

    private:

        SingleFilterFunction m_singleFilterFunction = nullptr;  ///< Function for single-buffer processing
        MultiFilterFunction m_multiFilterFunction = nullptr;   ///< Function for per-element processing
    };

    /**
     * @brief Predefined filters for common graphics operations
     * 
     * @details This namespace contains a collection of ready-to-use filters
     * for common graphics and terminal processing tasks. These filters provide
     * building blocks for complex visual effects and data transformations.
     * 
     * The filters are organized by their primary use case and input/output types,
     * making it easy to find appropriate filters for specific processing needs.
     */
    namespace filters
    {
        /**
         * @brief Data structure for vertex-based graphics processing
         * 
         * @details Contains all the information needed to process a single
         * vertex in the graphics pipeline. This is the primary data type
         * used for 2D graphics rendering and transformation.
         * 
         * The custom field allows filters to attach additional data specific
         * to their processing needs without modifying the base structure.
         */
        struct VertexData
        {
            Color color { 255, 0, 255, 255 };     ///< Vertex color with alpha
            Vector2<f32> position { 0.f, 0.f };   ///< Position in world coordinates
            Vector2<f32> uv { 0.f, 0.f };         ///< Texture coordinates (0-1 range)
            Vector2<f32> size { 0.f, 0.f };       ///< Size information for geometry
            Vector2<f32> inverseSize { 0.f, 0.f }; ///< Inverse size for optimization
            std::shared_ptr<void> custom = nullptr; ///< Custom data attachment point
        };

        /**
         * @brief Data for single character colored text rendering
         * 
         * @details Contains configuration for filters that convert colors
         * to colored character representations. Used for text-based graphics
         * rendering where each pixel becomes a colored character.
         */
        struct SingleCharacterColoredData : public BaseData
        {
            u32 codepoint;  ///< Unicode codepoint of the character to use
        };

        /**
         * @brief Filter that converts colors to colored character cells
         * 
         * @details Transforms color data into character cells using a single
         * character codepoint. The character is colored with the input color,
         * creating a simple text-based representation of the graphics.
         * 
         * This filter is useful for:
         * - Converting images to colored text art
         * - Creating retro text-mode graphics
         * - Simple color-to-character mapping
         * 
         * @par Example Usage:
         * @code
         * SingleCharacterColored charFilter('*');
         * colorPipeline.addFilter(&charFilter);
         * @endcode
         */
        struct SingleCharacterColored : public Filter<Color, CharacterCell, SingleCharacterColoredData>
        {
            /**
             * @brief Constructor with character codepoint
             * 
             * @details Creates a filter that uses the specified character
             * for all color-to-character conversions.
             * 
             * @param codepoint Unicode codepoint of the character to use
             */
            SingleCharacterColored(u32 codepoint);
        };

        /**
         * @brief Data for dithered color rendering with palette
         * 
         * @details Configuration for filters that apply dithering to reduce
         * color depth using a specified palette. Includes the target color
         * and available palette entries for dithering.
         */
        struct SingleColoredDitheredData : public BaseData
        {
            Color color;                          ///< Target color for dithering
            std::vector<u32> ditheringPalette;   ///< Available character codepoints for dithering
        };

        /**
         * @brief Filter that applies dithering with a character palette
         * 
         * @details Converts colors to character cells using dithering techniques
         * to approximate the target color with a limited character palette.
         * This creates visually appealing text representations of images.
         * 
         * The filter uses spatial dithering to distribute color information
         * across multiple characters, creating the illusion of intermediate
         * colors not directly available in the palette.
         * 
         * @par Use Cases:
         * - High-quality image to text conversion
         * - Retro graphics with limited character sets
         * - Artistic text-based rendering
         */
        struct SingleColoredDithered : public Filter<Color, CharacterCell, SingleColoredDitheredData>
        {
            /**
             * @brief Constructor with target color
             * 
             * @details Creates a dithering filter targeting the specified color.
             * The dithering palette should be set separately.
             * 
             * @param color Target color for dithering operations
             */
            SingleColoredDithered(Color color);
        };

        /**
         * @brief Data for animated character shuffling effects
         * 
         * @details Configuration for filters that randomly shuffle between
         * different characters over time. Provides timing control and
         * character palette management for animated text effects.
         */
        class CharacterShuffleColoredData : public BaseData
        {
        public:

            /**
             * @brief Set the character palette for shuffling
             * 
             * @details Assigns the list of characters that will be randomly
             * selected during the shuffling animation.
             * 
             * @param codepoints Vector of Unicode codepoints to shuffle between
             */
            void setCodepoints(const std::vector<u32>& codepoints);
            
            f32 shufflePeriod = 1.0f;  ///< Time interval between character changes (seconds)

        private:

            std::vector<u32> m_codepoints;                        ///< Available characters for shuffling
            mutable std::uniform_int_distribution<u32> m_distribution; ///< Random distribution for character selection
            mutable bool m_firstShuffle = true;                   ///< Flag for first shuffle initialization
            mutable bool m_shuffle = false;                       ///< Whether to shuffle this frame
            mutable f32 m_lastShuffleTime = 0.0f;                 ///< Last time shuffling occurred

        friend class CharacterShuffleColored;
        };

        /**
         * @brief Filter that creates animated character shuffling effects
         * 
         * @details Randomly changes characters over time to create dynamic
         * text effects. The filter maintains timing state and randomly selects
         * characters from a configured palette at specified intervals.
         * 
         * This filter is ideal for:
         * - Matrix-style digital rain effects
         * - Glitch text animations  
         * - Dynamic loading indicators
         * - Cyberpunk-style text effects
         * 
         * @par Example Usage:
         * @code
         * CharacterShuffleColored glitchFilter;
         * glitchFilter.data.setCodepoints({'0', '1', '#', '@', '$'});
         * glitchFilter.data.shufflePeriod = 0.1f;
         * @endcode
         */
        struct CharacterShuffleColored : public Filter<Color, CharacterCell, CharacterShuffleColoredData>
        {
            /**
             * @brief Default constructor
             * 
             * @details Creates a character shuffle filter with default settings.
             * Character palette must be set before use.
             */
            CharacterShuffleColored();

            /**
             * @brief Update timing for character shuffling
             * 
             * @details Called before each pipeline run to update the shuffle
             * timing and determine if characters should be changed this frame.
             */
            virtual void beforePipelineRun() override;
        };

        /**
         * @brief Data for solid color fill effects
         * 
         * @details Simple configuration containing a color value for filters
         * that apply uniform colors to vertex data.
         */
        struct SolidColorData : public BaseData
        {
            Color color;  ///< The solid color to apply
        };

        /**
         * @brief Filter that applies a solid color to vertex data
         * 
         * @details Sets all processed vertices to the same color value,
         * effectively creating a solid color fill effect. Useful for
         * creating backgrounds, overlays, and uniform coloring.
         * 
         * @par Example Usage:
         * @code
         * SolidColor redFill(Color::Red);
         * vertexPipeline.addFilter(&redFill);
         * @endcode
         */
        struct SolidColor : public Filter<VertexData, VertexData, SolidColorData>
        {
            /**
             * @brief Constructor with color specification
             * 
             * @details Creates a solid color filter with the specified color value.
             * 
             * @param color The color to apply to all processed vertices
             */
            SolidColor(Color color);
        };

        /**
         * @brief Filter that creates UV-based gradient effects
         * 
         * @details Generates color gradients based on UV coordinates,
         * creating smooth color transitions across surfaces. The gradient
         * direction and colors are determined by the UV mapping.
         * 
         * This filter is useful for:
         * - Background gradients
         * - Color transitions
         * - UV visualization
         * - Procedural coloring effects
         */
        struct UVGradient : public Filter<VertexData, VertexData>
        {
            /**
             * @brief Default constructor
             * 
             * @details Creates a UV gradient filter with default settings.
             * The gradient effect is based on UV coordinates.
             */
            UVGradient();
        };

        /**
         * @brief Filter that converts colors to grayscale
         * 
         * @details Transforms colored vertex data to grayscale values while
         * preserving luminance information. Uses standard luminance weighting
         * to maintain perceptual brightness consistency.
         * 
         * The filter applies the standard RGB to grayscale conversion:
         * Gray = 0.299*R + 0.587*G + 0.114*B
         * 
         * @par Use Cases:
         * - Black and white effects
         * - Luminance-based processing
         * - Retro monochrome graphics
         * - Accessibility improvements
         */
        struct Grayscale : public Filter<VertexData, VertexData>
        {
            /**
             * @brief Default constructor
             * 
             * @details Creates a grayscale filter with standard luminance weighting.
             */
            Grayscale();
        };

        /**
         * @brief Filter that inverts colors
         * 
         * @details Applies color inversion to vertex data by subtracting
         * each color component from its maximum value. Creates a negative
         * image effect while preserving alpha values.
         * 
         * The inversion formula: Output = MaxValue - Input
         * 
         * @par Use Cases:
         * - Negative image effects
         * - High contrast themes
         * - Visual accessibility
         * - Artistic effects
         */
        struct Invert : public Filter<VertexData, VertexData>
        {
            /**
             * @brief Default constructor
             * 
             * @details Creates a color inversion filter with standard behavior.
             */
            Invert();
        };

        /**
         * @brief Data for texture sampling operations
         * 
         * @details Configuration for filters that sample textures using
         * UV coordinates. Includes texture reference and sampling mode
         * for controlling interpolation behavior.
         */
        struct TextureSamplerData : public BaseData
        {
            Texture *texture;                                           ///< Texture to sample from
            Texture::SamplingMode samplingMode = Texture::SamplingMode::Bilinear; ///< Interpolation method
        };

        /**
         * @brief Filter that samples textures using UV coordinates
         * 
         * @details Reads color values from a texture using the UV coordinates
         * in vertex data. Supports different sampling modes for controlling
         * how texels are interpolated and filtered.
         * 
         * This filter is essential for:
         * - Texture mapping on surfaces
         * - Image-based effects
         * - Sprite rendering
         * - Material application
         * 
         * The filter uses the UV coordinates from input vertex data to
         * sample the texture and updates the vertex color with the
         * sampled result.
         * 
         * @par Example Usage:
         * @code
         * TextureSampler sampler(&myTexture);
         * sampler.data.samplingMode = Texture::SamplingMode::Nearest;
         * vertexPipeline.addFilter(&sampler);
         * @endcode
         */
        struct TextureSampler : public Filter<VertexData, VertexData, TextureSamplerData>
        {
            /**
             * @brief Constructor with texture reference
             * 
             * @details Creates a texture sampling filter for the specified texture.
             * Uses bilinear sampling by default.
             * 
             * @param texture Pointer to the texture to sample from
             * 
             * @warning The texture must remain valid for the filter's lifetime
             */
            TextureSampler(Texture *texture);
        };
    };

    template<typename T>
    u32 FilterableBuffer<T>::getSize() const {
        return m_buffer.size();
    }

    template<typename T>
    void FilterableBuffer<T>::setSize(u32 size) {
        m_buffer.resize(size);
    }

    template<typename T>
    void FilterableBuffer<T>::clear() {
        m_buffer.clear();
    }

    template<typename T>
    std::vector<T> &FilterableBuffer<T>::getBuffer() {
        return m_buffer;
    }

    template<typename T>
    T &FilterableBuffer<T>::operator[](u32 index) {
        return m_buffer[index];
    }

    template<typename T>
    const T &FilterableBuffer<T>::operator[](u32 index) const {
        return m_buffer[index];
    }

    template<typename InputType, typename OutputType, typename FilterData>
    Filter<InputType, OutputType, FilterData>::Filter() {
        inputType = std::type_index(typeid(InputType));
        outputType = std::type_index(typeid(OutputType));
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::apply(BaseFilterableBuffer *input, BaseFilterableBuffer *output) {
        auto [inputBuffer, outputBuffer] = getBufferPointers(input, output);

        if (executionMode == ExecutionMode::Single) {
            applySingle(inputBuffer, outputBuffer);
        } else if (executionMode == ExecutionMode::Sequential) {
            applySequential(inputBuffer, outputBuffer);
        } else if (executionMode == ExecutionMode::Concurrent) {
            applyConcurrent(inputBuffer, outputBuffer);
        }
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::setBaseData(const filters::BaseData &baseData) {
        data.time = baseData.time;
        data.buffer_resized = baseData.buffer_resized;
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::setSingleFilterFunction(SingleFilterFunction func) {
        m_singleFilterFunction = func;
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::setMultiFilterFunction(MultiFilterFunction func) {
        m_multiFilterFunction = func;
    }

    template<typename InputType, typename OutputType, typename FilterData>
    std::pair<FilterableBuffer<InputType> *, FilterableBuffer<OutputType> *> Filter<InputType, OutputType, FilterData>::getBufferPointers(BaseFilterableBuffer *input, BaseFilterableBuffer *output) {
        return {
            static_cast<FilterableBuffer<InputType> *>(input),
            static_cast<FilterableBuffer<OutputType> *>(output)
        };
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::applySingle(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer) {
        if (!m_singleFilterFunction) return;

        m_singleFilterFunction(*inputBuffer, *outputBuffer, data);
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::applySequential(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer) {
        if (!m_multiFilterFunction) return;

        for (u32 i = 0; i < inputBuffer->getSize(); ++i) {
            m_multiFilterFunction((*inputBuffer)[i], (*outputBuffer)[i], data);
        }
    }

    template<typename InputType, typename OutputType, typename FilterData>
    void Filter<InputType, OutputType, FilterData>::applyConcurrent(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer) {
        if (!m_multiFilterFunction) return;

        const u32 size = inputBuffer->getSize();
        const u32 numThreads = std::thread::hardware_concurrency();
        const u32 elementsPerThread = (size + numThreads - 1) / numThreads;

        std::vector<std::future<void>> futures;
        futures.reserve(numThreads);

        for (u32 threadId = 0; threadId < numThreads; ++threadId) {
            const u32 start = threadId * elementsPerThread;
            const u32 end = std::min(start + elementsPerThread, size);

            if (start < end) {
                futures.push_back(std::async(std::launch::async, [&, start, end]() {
                    for (u32 i = start; i < end; ++i) {
                        m_multiFilterFunction((*inputBuffer)[i], (*outputBuffer)[i], data);
                    }
                }));
            }
        }

        for (auto& future : futures) {
            future.wait();
        }
    }
}

#endif // TIL_FILTERS_HPP