#ifndef CGL_FILTER_PIPELINE_HPP
#define CGL_FILTER_PIPELINE_HPP

#include "filters.hpp"
#include "numeric_types.hpp"
#include <map>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace cgl
{
    template<typename InputType, typename OutputType>
    class FilterPipeline
    {
    public:

        class BufferRegistry
        {
        public:

            u32 registerBuffer(BaseFilterableBuffer *buffer);
            void unregisterBuffer(u32 id);

            BaseFilterableBuffer *getBuffer(u32 id);
            BaseFilterableBuffer *getFirstBuffer();

            u32 getBufferCount() const;

        private:

            u32 getNextId();

        private:

            std::map<u32, BaseFilterableBuffer *> m_buffers;
            std::set<u32> m_usedIds;
        };

        struct BufferSlot
        {
            std::type_index type = std::type_index(typeid(int));

            BaseFilterableBuffer *buffer = nullptr;
        };

    public:

        FilterPipeline &addFilter(BaseFilter *filter);
        FilterPipeline &insertFilter(u32 index, BaseFilter *filter);
        FilterPipeline &removeFilter(BaseFilter *filter);
        FilterPipeline &clearFilters();

        template<typename T>
        u32 addBuffer(FilterableBuffer<T> *buffer);
        template<typename T>
        void removeBuffer(u32 id);

        void build();

        void assignBufferToSlot(u32 slotIndex, u32 bufferId);

        void run(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer, const filters::BaseData &baseData);

    private:

        void checkFilterCompatibility();
        void createBufferSlots();
        void fillBufferSlots();

    private:

        bool built = true;

        std::vector<BaseFilter *> m_filters;
        std::vector<BufferSlot> m_buffers;
        std::unordered_map<std::type_index, BufferRegistry> m_bufferRegistries;
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
    FilterPipeline<InputType, OutputType> &FilterPipeline<InputType, OutputType>::removeFilter(BaseFilter *filter) {
        m_filters.erase(std::remove(m_filters.begin(), m_filters.end(), filter), m_filters.end());
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
            throw std::runtime_error("No buffer available for slot");
        }
    }

    template<typename InputType, typename OutputType>
    void FilterPipeline<InputType, OutputType>::run(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer, const filters::BaseData &baseData) {
        if (!built) {
            throw std::runtime_error("Pipeline is not built");
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
                    throw std::runtime_error("No buffer available for slot");
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
                throw std::runtime_error("Incompatible filter types");
            }
            expectedInput = filter->outputType;
        }
        if (expectedInput != std::type_index(typeid(OutputType))) {
            throw std::runtime_error("Incompatible filter types");
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
                throw std::runtime_error("No buffer available for slot");
            }
        }
    }
}

#endif // CGL_FILTER_PIPELINE_HPP