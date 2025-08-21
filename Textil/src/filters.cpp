#include "filters.hpp"

#include <iostream>

namespace til
{
    u32 BaseFilterableBuffer::getSize() const {
        return m_size;
    }

    void BaseFilterableBuffer::setSize(u32 size) {
        m_size = size;
    }

    void BaseFilterableBuffer::clear() {
        setSize(0);
    }

    namespace filters
    {
        SingleCharacterColored::SingleCharacterColored(u32 codepoint) {
            data.codepoint = codepoint;

            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<Color> &input, FilterableBuffer<CharacterCell> &output, const SingleCharacterColoredData &data) {
                for (u32 i = 0; i < input.getSize(); ++i) {
                    output[i].color = input[i];
                    output[i].codepoint = data.codepoint;
                }
            });

            setMultiFilterFunction([](Color &input, CharacterCell &output, const SingleCharacterColoredData &data) {
                output.color = input;
                output.codepoint = data.codepoint;
            });
        }

        void CharacterShuffleColoredData::setCodepoints(const std::vector<u32>& codepoints) {
            this->m_codepoints = codepoints;
            m_distribution = std::uniform_int_distribution<u32>(0, m_codepoints.size() - 1);
        }

        CharacterShuffleColored::CharacterShuffleColored() {
            data.setCodepoints({ 
                65, 66, 67, 68, 69,
                70, 71, 72, 73, 74,
                75, 76, 77, 78, 79,
                80, 81, 82, 83, 84,
                85, 86, 87, 88, 89, 90,
                97, 98, 99, 100, 101,
                102, 103, 104, 105, 106,
                107, 108, 109, 110, 111,
                112, 113, 114, 115, 116,
                117, 118, 119, 120, 121, 122,
                33, 35, 36, 37, 38, 42, 64,
                63, 47, 43, 45, 61, 94, 95,
                60, 62, 124
            });

            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<Color> &input, FilterableBuffer<CharacterCell> &output, const CharacterShuffleColoredData &data) {
                if (!data.m_shuffle) return;

                std::mt19937 engine(std::random_device{}());
                for (u32 i = 0; i < input.getSize(); ++i) {
                    output[i].color = input[i];
                    output[i].codepoint = data.m_codepoints[data.m_distribution(engine)];
                }
            });

            setMultiFilterFunction([](Color &input, CharacterCell &output, const CharacterShuffleColoredData &data) {
                if (!data.m_shuffle) return;

                thread_local std::mt19937 engine(std::random_device{}());
                output.color = input;
                output.codepoint = data.m_codepoints[data.m_distribution(engine)];
            });
        }

        void CharacterShuffleColored::beforePipelineRun() {
            data.m_shuffle = false;

            if (data.buffer_resized || data.time - data.m_lastShuffleTime > data.shufflePeriod || data.m_firstShuffle) {
                data.m_shuffle = true;
                data.m_lastShuffleTime = data.time;
                data.m_firstShuffle = false;
            }
        }

        SolidColor::SolidColor(Color color) {
            data.color = color;

            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<GeometryElementData> &input, FilterableBuffer<GeometryElementData> &output, const SolidColorData &data) {
                for (u32 i = 0; i < input.getSize(); ++i) {
                    output[i].color = data.color;
                }
            });

            setMultiFilterFunction([](GeometryElementData &input, GeometryElementData &output, const SolidColorData &data) {
                output.color = data.color;
            });
        }

        UVGradient::UVGradient() {
            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<GeometryElementData> &input, FilterableBuffer<GeometryElementData> &output, const BaseData &) {
                for (u32 i = 0; i < input.getSize(); ++i) {
                    output[i].color = sampleUVGradient(input[i].uv);
                }
            });

            setMultiFilterFunction([](GeometryElementData &input, GeometryElementData &output, const BaseData &) {
                output.color = sampleUVGradient(input.uv);
            });
        }

        Grayscale::Grayscale() {
            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<GeometryElementData> &input, FilterableBuffer<GeometryElementData> &output, const BaseData &) {
                for (u32 i = 0; i < input.getSize(); ++i) {
                    f32 luminance = input[i].color.luminance();
                    output[i].color = Color{
                        static_cast<u8>(luminance * 255.f),
                        static_cast<u8>(luminance * 255.f),
                        static_cast<u8>(luminance * 255.f),
                        255
                    };
                }
            });

            setMultiFilterFunction([](GeometryElementData &input, GeometryElementData &output, const BaseData &) {
                f32 luminance = input.color.luminance();
                output.color = Color{
                    static_cast<u8>(luminance * 255.f),
                    static_cast<u8>(luminance * 255.f),
                    static_cast<u8>(luminance * 255.f),
                    255
                };
            });
        }

        Invert::Invert() {
            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<GeometryElementData> &input, FilterableBuffer<GeometryElementData> &output, const BaseData &) {
                for (u32 i = 0; i < input.getSize(); ++i) {
                    output[i].color = input[i].color.inverted();
                }
            });

            setMultiFilterFunction([](GeometryElementData &input, GeometryElementData &output, const BaseData &) {
                output.color = input.color.inverted();
            });
        }

        TextureSampler::TextureSampler(Texture *texture) {
            data.texture = texture;

            executionMode = ExecutionMode::Concurrent;

            setSingleFilterFunction([](FilterableBuffer<GeometryElementData> &input, FilterableBuffer<GeometryElementData> &output, const TextureSamplerData &data) {
                for (u32 i = 0; i < input.getSize(); ++i) {
                    output[i].color = data.texture->sample(input[i].uv, data.samplingMode);
                }
            });

            setMultiFilterFunction([](GeometryElementData &input, GeometryElementData &output, const TextureSamplerData &data) {
                output.color = data.texture->sample(input.uv, data.samplingMode);
            });
        }
    }
}