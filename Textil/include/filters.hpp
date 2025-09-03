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
        struct BaseData
        {
            f32 time = 0.f;
            bool buffer_resized = false;
        };
    }

    class BaseFilterableBuffer
    {
    public:

        virtual ~BaseFilterableBuffer() = default;

        virtual u32 getSize() const;
        virtual void setSize(u32 size);
        virtual void clear();
    };

    template<typename T>
    class FilterableBuffer : public BaseFilterableBuffer
    {
    public:

        virtual u32 getSize() const override;

        virtual void setSize(u32 size) override;

        virtual void clear() override;

        std::vector<T> &getBuffer();

        T &operator[](u32 index);
        const T &operator[](u32 index) const;

    private:

        std::vector<T> m_buffer;
    };

    class BaseFilter
    {
    public:

        enum class ExecutionMode
        {
            Single,
            Sequential,
            Concurrent
        };

    public:

        ExecutionMode executionMode = ExecutionMode::Single;

        std::type_index inputType = std::type_index(typeid(int));
        std::type_index outputType = std::type_index(typeid(int));

    public:

        virtual ~BaseFilter() = default;
        virtual void beforePipelineRun() = 0;
        virtual void apply(BaseFilterableBuffer *input, BaseFilterableBuffer *output) = 0;
        virtual void afterPipelineRun() = 0;
        virtual void setBaseData(const filters::BaseData &baseData) = 0;
    };

    template<typename InputType, typename OutputType, typename FilterData = filters::BaseData>
    class Filter : public BaseFilter
    {
    public:

        using SingleFilterFunction = void (*)(FilterableBuffer<InputType>&, FilterableBuffer<OutputType>&, const FilterData&);
        using MultiFilterFunction = void (*)(InputType&, OutputType&, const FilterData&);

    public:

        FilterData data;

    public:

        Filter();

        virtual ~Filter() = default;

        virtual void beforePipelineRun() override {}
        virtual void apply(BaseFilterableBuffer *input, BaseFilterableBuffer *output) override final;
        virtual void afterPipelineRun() override {}
        virtual void setBaseData(const filters::BaseData &baseData) override final;

        void setSingleFilterFunction(SingleFilterFunction func);
        void setMultiFilterFunction(MultiFilterFunction func);

    private:

        std::pair<FilterableBuffer<InputType> *, FilterableBuffer<OutputType> *> getBufferPointers(BaseFilterableBuffer *input, BaseFilterableBuffer *output);

        void applySingle(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer);
        void applySequential(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer);
        void applyConcurrent(FilterableBuffer<InputType> *inputBuffer, FilterableBuffer<OutputType> *outputBuffer);

    private:

        SingleFilterFunction m_singleFilterFunction = nullptr;
        MultiFilterFunction m_multiFilterFunction = nullptr;
    };

    namespace filters
    {
        struct VertexData
        {
            Color color { 255, 0, 255, 255 };

            Vector2<f32> position { 0.f, 0.f };
            Vector2<f32> uv { 0.f, 0.f };
            Vector2<f32> size { 0.f, 0.f };
            Vector2<f32> inverseSize { 0.f, 0.f };

            std::shared_ptr<void> custom = nullptr;
        };

        struct SingleCharacterColoredData : public BaseData
        {
            u32 codepoint;
        };

        struct SingleCharacterColored : public Filter<Color, CharacterCell, SingleCharacterColoredData>
        {
            SingleCharacterColored(u32 codepoint);
        };

        struct SingleColoredDitheredData : public BaseData
        {
            Color color;
            std::vector<u32> ditheringPalette;
        };

        struct SingleColoredDithered : public Filter<Color, CharacterCell, SingleColoredDitheredData>
        {
            SingleColoredDithered(Color color);
        };

        class CharacterShuffleColoredData : public BaseData
        {
        public:

            void setCodepoints(const std::vector<u32>& codepoints);
            f32 shufflePeriod = 1.0f;

        private:

            std::vector<u32> m_codepoints;

            mutable std::uniform_int_distribution<u32> m_distribution;

            mutable bool m_firstShuffle = true;
            mutable bool m_shuffle = false;
            mutable f32 m_lastShuffleTime = 0.0f;

        friend class CharacterShuffleColored;
        };

        struct CharacterShuffleColored : public Filter<Color, CharacterCell, CharacterShuffleColoredData>
        {
            CharacterShuffleColored();

            virtual void beforePipelineRun() override;
        };

        struct SolidColorData : public BaseData
        {
            Color color;
        };

        struct SolidColor : public Filter<VertexData, VertexData, SolidColorData>
        {
            SolidColor(Color color);
        };

        struct UVGradient : public Filter<VertexData, VertexData>
        {
            UVGradient();
        };

        struct Grayscale : public Filter<VertexData, VertexData>
        {
            Grayscale();
        };

        struct Invert : public Filter<VertexData, VertexData>
        {
            Invert();
        };

        struct TextureSamplerData : public BaseData
        {
            Texture *texture;
            Texture::SamplingMode samplingMode = Texture::SamplingMode::Bilinear;
        };

        struct TextureSampler : public Filter<VertexData, VertexData, TextureSamplerData>
        {
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