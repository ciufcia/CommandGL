#ifndef CGL_FILTERS_HPP
#define CGL_FILTERS_HPP

#include "vector2.hpp"
#include "numeric_types.hpp"
#include <memory>
#include <vector>
#include "texture.hpp"

namespace cgl
{
    /**
     * @typedef FilterFunction
     * @brief Function pointer type for filter implementations.
     * 
     * Filter functions take two void pointers: one for filter-specific data
     * and one for pass-specific data that varies by filter type.
     * 
     * @param filterData Pointer to filter-specific configuration data.
     * @param passData Pointer to pass-specific data (pixel, screen buffer, etc.).
     */
    using FilterFunction = void (*)(void *filterData, void *passData);

    /**
     * @enum FilterType
     * @brief Enumeration of different filter execution modes.
     */
    enum class FilterType
    {
        SinglePass,   ///< Filter processes entire buffer at once
        Sequential,   ///< Filter processes pixels one by one in sequence
        Parallel,     ///< Filter processes pixels in parallel using OpenMP
    };

    /**
     * @struct Filter
     * @brief Represents a configurable filter that can be applied to rendering data.
     * 
     * Filters are the building blocks of the rendering pipeline, allowing for
     * post-processing effects, color manipulation, and custom rendering logic.
     * Each filter has a type that determines how it processes data, and can
     * store custom configuration data.
     */
    struct Filter
    {
        virtual ~Filter() = default;

        /**
         * @brief Factory method to create a filter with typed data.
         * @tparam T The type of the filter's configuration data.
         * @param type The execution mode for this filter.
         * @param function The function that implements the filter logic.
         * @param Data The configuration data for this filter instance.
         * @return A shared pointer to the created filter.
         */
        template<typename T>
        static std::shared_ptr<Filter> create(FilterType type, FilterFunction function, T Data);

        /**
         * @brief The execution mode that determines how this filter processes data.
         */
        FilterType type;

        /**
         * @brief Pointer to the function that implements this filter's logic.
         */
        FilterFunction function = nullptr;
        
        /**
         * @brief Shared pointer to filter-specific configuration data.
         * 
         * This data is passed to the filter function and can be any type
         * that was specified when creating the filter.
         */
        std::shared_ptr<void> data = nullptr;

        /**
         * @brief Whether this filter is currently active in the pipeline.
         */
        bool isEnabled = true;
        
        /**
         * @brief Human-readable name for this filter instance.
         */
        std::string name = "Unnamed Filter";
        
        /**
         * @brief Description of what this filter does.
         */
        std::string description = "No description provided.";
    };

    /**
     * @class FilterPipeline
     * @brief Manages an ordered collection of filters for sequential processing.
     * 
     * FilterPipeline provides a way to chain multiple filters together and execute
     * them in sequence. It supports adding, removing, and iterating through filters,
     * as well as runtime enable/disable of individual filters.
     */
    class FilterPipeline
    {
    public:

        /**
         * @brief Gets the total number of filters in the pipeline.
         * @return The number of filters currently in the pipeline.
         */
        u32 getFilterCount() const;

        /**
         * @brief Adds a filter to the pipeline at the specified position.
         * @param filter The filter to add to the pipeline.
         * @param position The position to insert the filter at.
         */
        void addFilter(std::shared_ptr<Filter> filter, u32 position);
        
        /**
         * @brief Removes the filter at the specified position.
         * @param position The position of the filter to remove.
         */
        void removeFilter(u32 position);
        
        /**
         * @brief Removes all filters from the pipeline.
         */
        void clearFilters();

    private:

        std::shared_ptr<Filter> getFilter(u32 position) const;

        void start();
        
        std::shared_ptr<Filter> getCurrentFilter() const;
        
        bool step();

        std::vector<std::shared_ptr<Filter>> m_filters;

    private:

        int m_currentFilterIndex = -1;
        bool m_isActive = false;

    friend class Drawable;
    friend class Framework;
    };

    /**
     * @namespace filter_pass_data
     * @brief Contains data structures passed to filters during different rendering passes.
     * 
     * This namespace defines the various data structures that are passed to filter
     * functions, providing context and data for different types of rendering operations.
     */
    namespace filter_pass_data
    {
        /**
         * @struct Base
         * @brief Base structure for all filter pass data types.
         * 
         * Provides common fields that are available to all filter types.
         */
        struct Base
        {
            float time = 0.f;  ///< Current time value for time-based effects
        };

        /**
         * @struct ScreenBufferSinglePass
         * @brief Pass data for filters that operate on the entire screen buffer at once.
         */
        struct ScreenBufferSinglePass : public Base
        {
            ScreenBuffer *screenBuffer = nullptr;  ///< Pointer to the screen buffer to process
        };

        /**
         * @struct PixelPass
         * @brief Pass data for filters that operate on individual pixels.
         * 
         * Contains all the information needed to process a single pixel,
         * including color, position, UV coordinates, and custom data.
         */
        struct PixelPass : public Base
        {
            Color color { 255, 0, 255, 255 };  ///< Current pixel color (defaults to magenta)

            Vector2<f32> position { 0.f, 0.f };     ///< Pixel position in screen coordinates
            Vector2<f32> uv { 0.f, 0.f };           ///< UV texture coordinates (0-1 range)
            Vector2<f32> size { 0.f, 0.f };         ///< Size of the drawable being rendered
            Vector2<f32> inverseSize { 0.f, 0.f };  ///< Reciprocal of size for optimization

            std::shared_ptr<void> custom = nullptr;  ///< Custom data for specialized filters
        };

        /**
         * @struct PixelSinglePass
         * @brief Pass data for filters that operate on entire pixel buffers at once.
         */
        struct PixelSinglePass : public Base
        {
            std::vector<PixelPass> *pixelBuffer = nullptr;  ///< Pointer to the pixel buffer to process
        };

        /**
         * @struct CharacterBufferSinglePass
         * @brief Pass data for filters that convert pixels to character representation.
         */
        struct CharacterBufferSinglePass : public Base
        {
            CharacterBuffer *characterBuffer = nullptr;  ///< Pointer to the character buffer to write to
            ScreenBuffer *screenBuffer = nullptr;        ///< Pointer to the source screen buffer
        };
    }

    /**
     * @brief Samples a color from UV coordinates.
     * @param uv The UV coordinates to sample (typically in 0-1 range).
     * @return The sampled color at the given UV coordinates.
     * 
     * This utility function provides a way to sample colors based on UV coordinates,
     * commonly used for texture mapping and procedural color generation.
     */
    Color sampleUVColor(const Vector2<f32> &uv);

    /**
     * @namespace filters
     * @brief Contains built-in filter implementations and their associated data structures.
     * 
     * This namespace provides a collection of ready-to-use filters for common
     * rendering operations, along with their configuration data structures.
     */
    namespace filters
    {
        /**
         * @brief Filter that sets all pixels to a single solid color.
         * @param filterData Pointer to SingleColorData containing the color to use.
         * @param passData Pointer to PixelPass data for the current pixel.
         */
        void singleColor(void *filterData, void *passData);

        /**
         * @struct SingleColorData
         * @brief Configuration data for the singleColor filter.
         */
        struct SingleColorData
        {
            Color color = { 255, 0, 255, 255 };  ///< The solid color to apply (defaults to magenta)
        };

        /**
         * @brief Filter that sets pixel color based on UV coordinates.
         * @param filterData Unused for this filter.
         * @param passData Pointer to PixelPass data for the current pixel.
         * 
         * Creates a visual representation of UV coordinates by mapping
         * them to specific colors.
         */
        void uv(void *filterData, void *passData);

        /**
         * @brief Filter that samples a texture and applies it to the screen buffer.
         * @param filterData Pointer to TextureData containing the texture and sampling mode.
         * @param passData Pointer to ScreenBufferSinglePass data for the current screen buffer.
         * 
         * This filter samples a texture and applies it to the screen buffer,
         * allowing for texture-based rendering effects.
         */
        void texture(void *filterData, void *passData);

        /**
         * @struct TextureData
         * @brief Configuration data for the texture filter.
         * 
         * This structure holds the texture to sample from and the sampling mode
         * to use when filtering the texture.
         */
        struct TextureData
        {
            std::shared_ptr<Texture> texture = nullptr;  ///< The texture to sample from
            Texture::SamplingMode sampling = Texture::SamplingMode::NearestNeighbor;  ///< Sampling mode for texture filtering
        };

        /**
         * @brief Filter that converts RGB colors to character representation.
         * @param filterData Pointer to RGBSingleCharacterData containing the character to use.
         * @param passData Pointer to CharacterBufferSinglePass data.
         * 
         * This filter converts pixel colors to character-based representation
         * suitable for console output.
         */
        void rgbSingleCharacter(void *filterData, void *passData);

        /**
         * @struct RGBSingleCharacterData
         * @brief Configuration data for the rgbSingleCharacter filter.
         */
        struct RGBSingleCharacterData
        {
            std::string character = "@";  ///< The character to use for representation (defaults to "@")
        };
    }

    template<typename T>
    std::shared_ptr<Filter> Filter::create(FilterType type, FilterFunction function, T data) {
        auto filter = std::make_shared<Filter>();

        filter->type = type;
        filter->function = function;
        filter->data = std::make_shared<T>(data);

        return filter;
    }
}

#endif // CGL_FILTERS_HPP