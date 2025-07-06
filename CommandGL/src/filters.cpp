#include "cgl.hpp"

#include <iostream>

namespace cgl
{
    u32 FilterPipeline::getFilterCount() const {
        return static_cast<u32>(m_filters.size());
    }

    void FilterPipeline::addFilter(std::shared_ptr<Filter> filter, u32 position) {
        if (position > m_filters.size())
            throw std::out_of_range("Position out of range");

        m_filters.insert(m_filters.begin() + position, filter);
    }

    void FilterPipeline::removeFilter(u32 position) {
        if (position >= m_filters.size())
            throw std::out_of_range("Position out of range");

        m_filters.erase(m_filters.begin() + position);
    }

    void FilterPipeline::clearFilters() {
        m_filters.clear();
    }

    std::shared_ptr<Filter> FilterPipeline::getFilter(u32 position) const {
        if (position >= m_filters.size())
            throw std::out_of_range("Position out of range");

        return m_filters[position];
    }

    void FilterPipeline::start() {
        m_currentFilterIndex = -1;
        m_isActive = true;
    }

    std::shared_ptr<Filter> FilterPipeline::getCurrentFilter() const {
        if (m_currentFilterIndex >= m_filters.size())
            return nullptr;

        return m_filters[m_currentFilterIndex];
    }

    bool FilterPipeline::step() {
        if (!m_isActive) {
            return false;
        }

        while (true) {
            m_currentFilterIndex++;

            if (m_currentFilterIndex >= m_filters.size()) {
                m_isActive = false;
                return false;
            }

            if (m_filters[m_currentFilterIndex] && m_filters[m_currentFilterIndex]->isEnabled) {
                return true;
            }
        }
    }

    Color sampleUVColor(const Vector2<f32> &uv) {
        return {
            static_cast<u8>(std::lerp(0.f, 255.f, uv.y)),
            static_cast<u8>(std::lerp(0.f, 255.f, uv.x)),
            static_cast<u8>(std::lerp(255.f, 0.f, uv.x))
        };
    }

    namespace filters
    {
        void singleColor(void *filterData, void *passData) {
            auto castedFilterData = static_cast<SingleColorData *>(filterData);
            auto castedPassData = static_cast<filter_pass_data::PixelPass *>(passData);

            castedPassData->color = castedFilterData->color;
        }

        void uv(void *filterData, void *passData) {
            auto castedPassData = static_cast<filter_pass_data::PixelPass *>(passData);

            castedPassData->color = sampleUVColor(castedPassData->uv);
        }

        void texture(void *filterData, void *passData) {
            auto castedFilterData = static_cast<TextureData *>(filterData);
            auto castedPassData = static_cast<filter_pass_data::PixelPass *>(passData);

            castedPassData->color = castedFilterData->texture->sample(castedPassData->uv, castedFilterData->sampling);
        }

        void rgbSingleCharacter(void *filterData, void *passData) {
            auto castedFilterData = static_cast<RGBSingleCharacterData *>(filterData);
            auto castedPassData = static_cast<filter_pass_data::CharacterBufferSinglePass *>(passData);

            std::wstring &characters = castedPassData->characterBuffer->getCharacters();
            std::vector<Color> &colors = castedPassData->screenBuffer->getBuffer();
            
            characters.clear();

            Color currentColor = colors[0];

            characters += L"\x1b[38;2;" +
                          std::to_wstring(currentColor.r) +
                          L";" +
                          std::to_wstring(currentColor.g) +
                          L";" +
                          std::to_wstring(currentColor.b) +
                          L"m";

            for (int y = 0; y < castedPassData->screenBuffer->getSize().y; y++) {
                for (int x = 0; x < castedPassData->screenBuffer->getSize().x; x++) {
                    if (colors[y * castedPassData->screenBuffer->getSize().x + x] != currentColor) {
                        currentColor = colors[y * castedPassData->screenBuffer->getSize().x + x];
                        characters += L"\x1b[38;2;" +
                                      std::to_wstring(currentColor.r) +
                                      L";" +
                                      std::to_wstring(currentColor.g) +
                                      L";" +
                                      std::to_wstring(currentColor.b) +
                                      L"m";
                    }

                    characters += L"@";   
                }

                characters += L"\x1b[1E\x1b[0G";
            }

            characters += L"\x1b[0m";
        }
    }
}