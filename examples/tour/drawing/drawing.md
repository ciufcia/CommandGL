<div align="center">

# 4. Drawing

## [Back to Table of Contents](../table_of_contents.md)

</div>

This chapter will cover drawing objects on the screen, as well as applying effects on them. In this chapter we will be modifying the code from the first chapter, which can be found [here](../setup/basic_app.cpp).

## 1. The drawing pipeline

To draw our object onto the screen, we need to go through 3 steps:

- Applying the affine transformations
- Generating the pixels that will represent our object (generating geometry)
- Coloring these pixels via some rule (applying a filter)

The `Drawable` class encapsulates these 3 steps by providing:

- A `Transform` class
- A generateGeometry() method
- A filter pipeline

## 2. Geometry generation

Any custom drawable must inherit from the `Drawable` class, thus providing an override of the `generateGeometry()` method. The goal of the method is to fill the passed drawable buffer with data describing the pixels sto be colored. The data for each pixel is represented by the `PixelPass` struct. The framework also expects for the transform passed into the method to be applied onto the drawable. Here's a basic example of a geometry generator used by the `Point` drawable:

```cpp
void drawables::Point::generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) {
    // apply the transform
    Vector2<f32> transformedPosition = transform.getMatrix() * position;

    // fill the data for the single pixel that will represent the point
    filter_pass_data::PixelPass pixelData;
    pixelData.position = transformedPosition;
    pixelData.uv = { 0.f, 0.f };
    pixelData.size = { 1.f, 1.f };
    pixelData.inverseSize = { 1.f, 1.f };

    // push the data onto the drawable buffer
    drawableBuffer.push_back(pixelData);
}
```

## 3. Filters

CommandGL colors it's drawables by applying [filters](https://ciufcia.github.io/CommandGL/structcgl_1_1Filter.html) to them. Each filter has one of three types: `SinglePass`, `Sequential`, `Parallel`.

- SinglePass filters are ran only once, handling the entire drawable buffer at once.
- Sequential filters are ran for each generated pixel in a continuos fashion
- Parallel filters are ran for each generated pixel in a parallel fashion (drawable buffer access isn't guaranteed to be thread-safe!)

The second important part of a filter is the filter function, defined like so:

```cpp
void (*)(void *filterData, void *passData)
```

where `filterData` is the `data` member of the `Filter` class, which allows us to include some additional data that may be important for applying the filter, eg. a color to fill the pixels with. `passData` points to a struct containing data relevant to currently processed pixel/buffer. In the cases of sequential and parallel filters the type pointed to will be `PixelPass`, while in the case of a SinglePass filter, the drawableBuffer will be passed. Here's a filter function that will color the pixels based on their UV coordinates:

```cpp
...

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

...
```

## 4. Combining the pieces

Now that we've discussed the 3 steps individually, it's time to combine them together to draw our first triangle!

First we need to create a drawable. As to not overcomplicate the tutorial we will be using cgl's built in `Triangle` drawable with an already implemented `generateGeometry()`:

```cpp
...

auto triangle = cgl::Drawable::create<cgl::drawables::Triangle>(
    cgl::Vector2<cgl::f32>(0.f, 0.f),
    cgl::Vector2<cgl::f32>(1.0f, 0.f),
    cgl::Vector2<cgl::f32>(0.5f, 1.0f)
);

...
```

Let's scale it up a bit so it's more visible:

```cpp
...

triangle->transform.setScale({ 20.f, 20.f });

...
```

Now let's draw the triangle after we clear the display:

```cpp
...

framework.clearDisplay({255, 255, 255});

framework.draw(triangle);

framework.update();

...
```

Now when you run the program, you should see a magenta triangle being displayed.

Let's now color our triangle with the uv gradient, we've created earlier.

Firstly we need to create a `Filter` object:

```cpp
...

auto uvFilterObject = cgl::Filter::create(
    cgl::FilterType::Parallel,
    uvFilter,
    nullptr // if we wanted to, we could pass some data here, but we don't need it for this filter
);

...
```

Then we can insert our filter into the triangles fragment pipeline:

```cpp
...

triangle->fragmentPipeline.addFilter(uvFilterObject, 0);

...
```

After running this our triangle should be filled with a nice gradient.

## 5. Chaining filters

The fragment pipeline allows for applying multiple filters to the object every draw call. Let's now make our gradient monochromatic, by creating a monochromatic filter:

```cpp
...

void monochromaticFilter(void *filterData, void *passData) {
    auto castedPassData = static_cast<cgl::filter_pass_data::PixelPass *>(passData);

    cgl::u8 luminance = castedPassData->color.luminance() * 255;
    castedPassData->color = { luminance, luminance, luminance, castedPassData->color.a };
}

...

    auto monochromaticFilterObject = cgl::Filter::create(
        cgl::FilterType::Parallel,
        monochromaticFilter,
        nullptr // no data needed for this filter either
    );

...

    triangle->fragmentPipeline.addFilter(monochromaticFilterObject, 1 /*This filer will be ran after the uv filter*/);

...
```

Now run the program and check out the result for yourself!

## 6. Screen filters

You can also apply filters on the entire screen, after each drawable has been drawn. Let's now change our code a bit, so that the monochromatic filter gets applied to the whole screen, instead of just our triangle.

```cpp
...

framework.screenFilterPipeline.addFilter(monochromaticFilterObject, 0);

...
```

## 7. Character filter pipeline

The framework also uses another filter pipeline which can be accessed by: `framework.characterFilterPipeline`. The job of this pipeline is to convert the screen's pixel data into a string that can be displayed in the console. This pipeline won't be explained further in this tutorial.

<div align="center">

# [Back to Table of Contents](../table_of_contents.md) | [Go to the Top](#4-drawing) | [Code from this Chapter](drawing.cpp)

</div>