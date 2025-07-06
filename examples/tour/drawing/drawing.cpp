#include <cgl.hpp>

cgl::Color sampleUV(const cgl::Vector2<cgl::f32> &uv) {
    return {
        static_cast<cgl::u8>(std::lerp(0.f, 255.f, uv.y)),
        static_cast<cgl::u8>(std::lerp(0.f, 255.f, uv.x)),
        static_cast<cgl::u8>(std::lerp(255.f, 0.f, uv.x))
    };
}

void uvFilter(void *filterData, void *passData) {
    auto castedPassData = static_cast<cgl::filter_pass_data::PixelPass *>(passData);

    castedPassData->color = sampleUV(castedPassData->uv);
}

void monochromaticFilter(void *filterData, void *passData) {
    auto castedPassData = static_cast<cgl::filter_pass_data::PixelPass *>(passData);

    cgl::u8 luminance = castedPassData->color.luminance() * 255;
    castedPassData->color = { luminance, luminance, luminance, castedPassData->color.a };
}

int main() {
    cgl::Framework framework;

    framework.initialize();

    auto uvFilterObject = cgl::Filter::create(
        cgl::FilterType::Parallel,
        uvFilter,
        nullptr // if we wanted to, we could pass some data here, but we don't need it for this filter
    );

    auto monochromaticFilterObject = cgl::Filter::create(
        cgl::FilterType::Parallel,
        monochromaticFilter,
        nullptr // no data needed for this filter either
    );

    framework.screenFilterPipeline.addFilter(monochromaticFilterObject, 0);

    auto triangle = cgl::Drawable::create<cgl::drawables::Triangle>(
        cgl::Vector2<cgl::f32>(0.f, 0.f),
        cgl::Vector2<cgl::f32>(1.0f, 0.f),
        cgl::Vector2<cgl::f32>(0.5f, 1.0f)
    );

    triangle->transform.setScale({ 20.f, 20.f });

    triangle->fragmentPipeline.addFilter(uvFilterObject, 0);

    bool running = true;

    while (running) {
        framework.eventManager.handleEvents(
            [&](cgl::KeyPressEvent, const cgl::Event &event) {
                if (event.key == cgl::KeyCode::Escape) {
                    running = false;
                }
            }
        );

        framework.clearDisplay({255, 255, 255});

        framework.draw(triangle);

        framework.update();
    }
}