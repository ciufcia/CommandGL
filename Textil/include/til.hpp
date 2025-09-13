/**
 * @file til.hpp
 * @brief Primary aggregate header for the Textil terminal graphics library
 * 
 * @mainpage Textil - Terminal Graphics Library
 * 
 * @section intro_sec Introduction
 * 
 * Textil is a comprehensive C++ library for creating graphics applications that run in terminal environments.
 * It provides a modern, object-oriented interface for terminal manipulation, 2D graphics rendering, 
 * input handling, and window management. The library is designed to work across Windows, Linux, and macOS 
 * platforms, offering a unified API that abstracts platform-specific terminal operations.
 * 
 * @section features_sec Key Features
 * 
 * - **Cross-platform terminal graphics**: Unified API works on Windows, Linux, and macOS
 * - **2D rendering pipeline**: Hardware-accelerated-style rendering with software implementation
 * - **Advanced color support**: RGBA colors with multiple blending modes and gradients  
 * - **Flexible input handling**: Keyboard, mouse, and console event processing
 * - **Window management**: Multi-layered window system with depth sorting
 * - **Bitmap font rendering**: BDF font support for pixel-perfect text rendering
 * - **Texture system**: Image loading, manipulation, and sampling with filtering
 * - **Filter pipeline**: Customizable rendering effects and post-processing
 * - **High-precision timing**: Frame rate control and performance monitoring
 * - **Comprehensive error handling**: Structured error system with detailed reporting
 * 
 * @section getting_started_sec Getting Started
 * 
 * @subsection basic_usage Basic Usage Pattern
 * 
 * ```cpp
 * #include <til.hpp>
 * 
 * int main() {
 *     til::Framework framework;
 *     framework.initialize();
 *     
 *     // Create a window
 *     auto window = framework.windowManager.createWindow({0, 0}, {80, 24});
 *     
 *     // Main application loop
 *     bool running = true;
 *     while (running) {
 *         framework.update();
 *         
 *         // Process events
 *         for (const auto& event : framework.eventManager.getEvents()) {
 *             if (event.isOfType<til::KeyPressEvent>() && event.key == til::KeyCode::Escape) {
 *                 running = false;
 *             }
 *         }
 *         
 *         // Render graphics
 *         window->clear();
 *         // ... your rendering code here ...
 *         
 *         framework.display();
 *     }
 *     
 *     return 0;
 * }
 * ```
 * 
 * @subsection key_concepts Key Concepts
 * 
 * - **Framework**: Central coordinator managing all subsystems
 * - **Windows**: Rendering targets with independent coordinate systems
 * - **Events**: Type-safe input event handling with unified key codes
 * - **Textures**: 2D image containers with filtering and sampling
 * - **Colors**: RGBA representation with advanced blending operations
 * - **Vectors/Matrices**: Mathematical primitives for 2D transformations
 * - **Filters**: Rendering pipeline effects for visual enhancements
 * 
 * @section architecture_sec Architecture Overview
 * 
 * Textil is organized into several key subsystems:
 * 
 * @subsection core_types Core Mathematical Types
 * - `Vector2<T>`: 2D vector operations and transformations
 * - `Matrix3<T>`: 3x3 matrices for 2D transformations  
 * - `Color`: RGBA color with blending operations
 * - Type aliases in `numeric_types.hpp` for consistent numeric types
 * 
 * @subsection rendering_subsystem Rendering Subsystem
 * - `Renderer`: Core graphics pipeline and primitive rendering
 * - `Texture`: Image storage, loading, and sampling
 * - `RenderTarget`: Configurable rendering destinations
 * - Filter pipeline for customizable visual effects
 * 
 * @subsection input_subsystem Input Subsystem  
 * - `Event`: Unified input event representation
 * - `EventManager`: Event queue processing and distribution
 * - `KeyCode`: Cross-platform key code enumeration
 * - Platform-specific input device integration
 * 
 * @subsection windowing_subsystem Windowing Subsystem
 * - `Window`: Independent rendering contexts with transforms
 * - `WindowManager`: Multi-window coordination and depth sorting
 * - `Console`: Low-level terminal interface abstraction
 * 
 * @subsection text_subsystem Text Rendering
 * - `BitmapFont`: BDF font loading and glyph management
 * - Unicode support via UTF-8 text processing
 * - Texture-based text rendering for high quality output
 * 
 * @subsection timing_subsystem Timing and Performance
 * - `Clock`: High-precision timing measurements
 * - Frame rate limiting and performance monitoring
 * - Duration conversion utilities for different time units
 * 
 * @section platform_support Platform Support
 * 
 * Textil provides full cross-platform compatibility:
 * 
 * - **Windows**: Uses WinAPI for console and input handling
 * - **Linux**: Uses termios and evdev for terminal and input control
 * - **macOS**: Uses HID framework for input and terminal manipulation
 * 
 * Platform-specific code is isolated and abstracted through unified interfaces,
 * allowing applications to run unchanged across different operating systems.
 * 
 * @section performance_considerations Performance Considerations
 * 
 * - **Frame rate limiting**: Use `Framework::setTargetUpdateRate()` for consistent timing
 * - **Texture memory**: Large textures consume significant memory; consider atlasing
 * - **Filter pipeline**: Complex filter chains can impact rendering performance  
 * - **Event processing**: Process events efficiently to maintain responsiveness
 * - **Window count**: Minimize active windows for optimal performance
 * 
 * @section thread_safety Thread Safety
 * 
 * Textil is generally not thread-safe and should be used from a single thread.
 * The error handling system provides thread-safe configuration through atomic operations.
 * For multi-threaded applications, coordinate access to Textil objects externally.
 * 
 * @section license_sec License
 * 
 * [Add your license information here]
 * 
 * @namespace til
 * @brief Main namespace containing all Textil library functionality
 */

// Primary aggregate header for Textil
#ifndef TIL_TIL_HPP
#define TIL_TIL_HPP

#define NOMINMAX

// Core mathematical types and utilities
#include "vector2.hpp"
#include "numeric_types.hpp"
#include "matrix3.hpp"
#include "color.hpp"

// Platform abstraction and system interfaces  
#include "console.hpp"
#include "global_memory.hpp"
#include "errors.hpp"

// Input and event handling
#include "event.hpp"
#include "keycodes.hpp"
#include "event_manager.hpp"

// Rendering and graphics pipeline
#include "filters.hpp"
#include "filter_pipeline.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "render.hpp"
#include "drawables.hpp"

// Windowing and display management
#include "character_cell.hpp"
#include "window.hpp"
#include "window_manager.hpp"

// Text rendering and fonts
#include "text.hpp"

// Timing and performance
#include "timing.hpp"

// Main framework coordination
#include "framework.hpp"

#endif // TIL_TIL_HPP
