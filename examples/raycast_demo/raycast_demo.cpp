#include <til.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>

namespace
{
    constexpr int kMapWidth = 16;
    constexpr int kMapHeight = 16;

    using MapGrid = std::array<int, kMapWidth * kMapHeight>;

    constexpr MapGrid kLevel = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 1,
        1, 0, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 1,
        1, 0, 2, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 4, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 4, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1,
        1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 4, 0, 1,
        1, 0, 3, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 4, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    int getCell(int x, int y)
    {
        if (x < 0 || x >= kMapWidth || y < 0 || y >= kMapHeight) {
            return 1;
        }
        return kLevel[static_cast<std::size_t>(y * kMapWidth + x)];
    }

    bool isWalkable(const til::Vector2<til::f32> &position)
    {
        const int mapX = static_cast<int>(position.x);
        const int mapY = static_cast<int>(position.y);
        return getCell(mapX, mapY) == 0;
    }

    til::Color scaleColor(const til::Color &color, til::f32 brightness)
    {
        const auto scaleChannel = [brightness](til::u8 value) {
            const til::f32 scaled = std::clamp(brightness * static_cast<til::f32>(value), 0.0f, 255.0f);
            return static_cast<til::u8>(scaled);
        };

        return {
            scaleChannel(color.r),
            scaleChannel(color.g),
            scaleChannel(color.b),
            color.a
        };
    }

    til::Color shadeWall(int cellId, bool isSideHit, til::f32 distance)
    {
        til::Color baseColor;
        switch (cellId) {
            case 1: baseColor = { 220, 60, 60, 255 }; break;
            case 2: baseColor = { 60, 190, 210, 255 }; break;
            case 3: baseColor = { 245, 200, 65, 255 }; break;
            case 4: baseColor = { 170, 90, 220, 255 }; break;
            default: baseColor = { 210, 210, 210, 255 }; break;
        }

        til::f32 brightness = 1.0f / (1.0f + distance * 0.18f);
        brightness = std::clamp(brightness, 0.18f, 1.0f);

        if (isSideHit) {
            brightness *= 0.75f;
        }

        return scaleColor(baseColor, brightness);
    }

    til::Color lerpColor(const til::Color &from, const til::Color &to, til::f32 t)
    {
        return {
            static_cast<til::u8>(std::lerp(static_cast<til::f32>(from.r), static_cast<til::f32>(to.r), t)),
            static_cast<til::u8>(std::lerp(static_cast<til::f32>(from.g), static_cast<til::f32>(to.g), t)),
            static_cast<til::u8>(std::lerp(static_cast<til::f32>(from.b), static_cast<til::f32>(to.b), t)),
            static_cast<til::u8>(std::lerp(static_cast<til::f32>(from.a), static_cast<til::f32>(to.a), t))
        };
    }
}

int main()
{
    til::Framework framework;
    framework.initialize();
    framework.setTargetUpdateRate(60);

    til::Window &window = framework.windowManager.createWindow();
    window.setSize(framework.console.getSize());
    window.setRenderer(&framework.renderer);
    window.depth = 1.0f;

    til::filters::SingleCharacterColored charFilter(0x2588); // solid block for filled pixels
    window.characterPipeline.addFilter(&charFilter).build();

    struct Player
    {
    til::Vector2<til::f32> position { 8.5f, 8.5f };
    til::Vector2<til::f32> direction { -1.0f, 0.0f };
        til::Vector2<til::f32> cameraPlane { 0.0f, 0.66f };
        til::f32               fovScale { 0.66f };

        void updateCameraPlane()
        {
            cameraPlane = { direction.y * fovScale, -direction.x * fovScale };
        }
    } player;
    player.updateCameraPlane();

    bool moveForward = false;
    bool moveBackward = false;
    bool strafeLeft = false;
    bool strafeRight = false;
    bool turnLeft = false;
    bool turnRight = false;
    bool running = true;

    while (running) {
        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<til::ConsoleEvent>()) {
                window.setSize(event->newSize);
                player.updateCameraPlane();
                continue;
            }

            if (event->isOfType<til::KeyPressEvent>()) {
                switch (event->key) {
                    case til::KeyCode::Escape: running = false; break;
                    case til::KeyCode::W: moveForward = true; break;
                    case til::KeyCode::S: moveBackward = true; break;
                    case til::KeyCode::A: strafeLeft = true; break;
                    case til::KeyCode::D: strafeRight = true; break;
                    case til::KeyCode::Left:
                    case til::KeyCode::Q: turnLeft = true; break;
                    case til::KeyCode::Right:
                    case til::KeyCode::E: turnRight = true; break;
                    default: break;
                }
            } else if (event->isOfType<til::KeyReleaseEvent>()) {
                switch (event->key) {
                    case til::KeyCode::W: moveForward = false; break;
                    case til::KeyCode::S: moveBackward = false; break;
                    case til::KeyCode::A: strafeLeft = false; break;
                    case til::KeyCode::D: strafeRight = false; break;
                    case til::KeyCode::Left:
                    case til::KeyCode::Q: turnLeft = false; break;
                    case til::KeyCode::Right:
                    case til::KeyCode::E: turnRight = false; break;
                    default: break;
                }
            }
        }

        const til::f32 deltaSeconds = std::min(til::getDurationInSeconds(framework.getLastUpdateDuration()), 0.1f);
        const til::f32 moveSpeed = 3.5f * deltaSeconds;
        const til::f32 strafeSpeed = 3.0f * deltaSeconds;
    const til::f32 rotationSpeed = 2.5f * deltaSeconds;

        if (moveForward) {
            til::Vector2<til::f32> step {
                player.direction.x * moveSpeed,
                player.direction.y * moveSpeed
            };
            til::Vector2<til::f32> candidateX { player.position.x + step.x, player.position.y };
            til::Vector2<til::f32> candidateY { player.position.x, player.position.y + step.y };
            if (isWalkable(candidateX)) {
                player.position.x = candidateX.x;
            }
            if (isWalkable(candidateY)) {
                player.position.y = candidateY.y;
            }
        }
        if (moveBackward) {
            til::Vector2<til::f32> step {
                -player.direction.x * moveSpeed,
                -player.direction.y * moveSpeed
            };
            til::Vector2<til::f32> candidateX { player.position.x + step.x, player.position.y };
            til::Vector2<til::f32> candidateY { player.position.x, player.position.y + step.y };
            if (isWalkable(candidateX)) {
                player.position.x = candidateX.x;
            }
            if (isWalkable(candidateY)) {
                player.position.y = candidateY.y;
            }
        }

        if (strafeLeft || strafeRight) {
            const til::f32 direction = strafeRight ? 1.0f : -1.0f;
            til::Vector2<til::f32> strafe {
                player.direction.y * direction * strafeSpeed,
                -player.direction.x * direction * strafeSpeed
            };
            til::Vector2<til::f32> candidateX { player.position.x + strafe.x, player.position.y };
            til::Vector2<til::f32> candidateY { player.position.x, player.position.y + strafe.y };
            if (isWalkable(candidateX)) {
                player.position.x = candidateX.x;
            }
            if (isWalkable(candidateY)) {
                player.position.y = candidateY.y;
            }
        }

        auto rotatePlayer = [&player](til::f32 angle) {
            const til::f32 cosAngle = std::cos(angle);
            const til::f32 sinAngle = std::sin(angle);

            const til::f32 oldDirX = player.direction.x;
            const til::f32 oldDirY = player.direction.y;
            player.direction.x = oldDirX * cosAngle - oldDirY * sinAngle;
            player.direction.y = oldDirX * sinAngle + oldDirY * cosAngle;
            player.updateCameraPlane();
        };

        if (turnLeft) {
            rotatePlayer(rotationSpeed);
        }
        if (turnRight) {
            rotatePlayer(-rotationSpeed);
        }

        const auto size = window.getSize();
        if (size.x == 0 || size.y == 0) {
            framework.display();
            framework.update();
            continue;
        }

        const til::u32 horizon = size.y / 2;

        const til::Color skyTop { 30, 60, 120, 255 };
        const til::Color skyBottom { 80, 120, 200, 255 };
        const til::Color floorNear { 55, 48, 38, 255 };
        const til::Color floorFar { 10, 10, 14, 255 };

        for (til::u32 y = 0; y < horizon; ++y) {
            const til::f32 t = static_cast<til::f32>(y) / std::max<til::u32>(1, horizon - 1);
            const til::Color rowColor = lerpColor(skyTop, skyBottom, t);
            for (til::u32 x = 0; x < size.x; ++x) {
                framework.renderer.drawImmediatePixel(window, { x, y }, rowColor, til::BlendMode::None);
            }
        }

        for (til::u32 y = horizon; y < size.y; ++y) {
            const til::f32 t = static_cast<til::f32>(y - horizon) / std::max<til::u32>(1, size.y - horizon - 1);
            const til::Color rowColor = lerpColor(floorNear, floorFar, t);
            for (til::u32 x = 0; x < size.x; ++x) {
                framework.renderer.drawImmediatePixel(window, { x, y }, rowColor, til::BlendMode::None);
            }
        }

        for (til::u32 x = 0; x < size.x; ++x) {
            const til::f32 cameraX = 2.0f * static_cast<til::f32>(x) / static_cast<til::f32>(size.x) - 1.0f;
            til::Vector2<til::f32> rayDirection {
                player.direction.x + player.cameraPlane.x * cameraX,
                player.direction.y + player.cameraPlane.y * cameraX
            };

            int mapX = static_cast<int>(player.position.x);
            int mapY = static_cast<int>(player.position.y);

            til::f32 sideDistanceX;
            til::f32 sideDistanceY;

            const til::f32 deltaDistanceX = rayDirection.x == 0.0f ? std::numeric_limits<til::f32>::infinity() : std::fabs(1.0f / rayDirection.x);
            const til::f32 deltaDistanceY = rayDirection.y == 0.0f ? std::numeric_limits<til::f32>::infinity() : std::fabs(1.0f / rayDirection.y);

            int stepX;
            int stepY;

            if (rayDirection.x < 0.0f) {
                stepX = -1;
                sideDistanceX = (player.position.x - static_cast<til::f32>(mapX)) * deltaDistanceX;
            } else {
                stepX = 1;
                sideDistanceX = (static_cast<til::f32>(mapX + 1) - player.position.x) * deltaDistanceX;
            }

            if (rayDirection.y < 0.0f) {
                stepY = -1;
                sideDistanceY = (player.position.y - static_cast<til::f32>(mapY)) * deltaDistanceY;
            } else {
                stepY = 1;
                sideDistanceY = (static_cast<til::f32>(mapY + 1) - player.position.y) * deltaDistanceY;
            }

            bool hit = false;
            bool sideHit = false;
            int cellId = 0;

            for (int iterations = 0; iterations < 128 && !hit; ++iterations) {
                if (sideDistanceX < sideDistanceY) {
                    sideDistanceX += deltaDistanceX;
                    mapX += stepX;
                    sideHit = false;
                } else {
                    sideDistanceY += deltaDistanceY;
                    mapY += stepY;
                    sideHit = true;
                }

                cellId = getCell(mapX, mapY);
                if (cellId > 0) {
                    hit = true;
                }
            }

            if (!hit) {
                continue;
            }

            const til::f32 perpendicularDistance = sideHit ? (sideDistanceY - deltaDistanceY) : (sideDistanceX - deltaDistanceX);
            const til::f32 clampedDistance = std::max(perpendicularDistance, 0.0001f);

            const til::f32 projectedHeight = static_cast<til::f32>(size.y) / clampedDistance;
            int lineHeight = static_cast<int>(projectedHeight);
            int drawStart = static_cast<int>(static_cast<int>(size.y) / 2 - lineHeight / 2);
            int drawEnd = static_cast<int>(static_cast<int>(size.y) / 2 + lineHeight / 2);

            drawStart = std::max(drawStart, 0);
            drawEnd = std::min(drawEnd, static_cast<int>(size.y) - 1);

            const til::Color columnColor = shadeWall(cellId, sideHit, clampedDistance);

            for (int y = drawStart; y <= drawEnd; ++y) {
                framework.renderer.drawImmediatePixel(
                    window,
                    { static_cast<til::u32>(x), static_cast<til::u32>(y) },
                    columnColor,
                    til::BlendMode::None
                );
            }
        }

        const auto drawMinimap = [&](const til::Vector2<til::f32> &playerPos, const til::Vector2<til::f32> &playerDir) {
            constexpr til::u32 offsetX = 2;
            constexpr til::u32 offsetY = 2;
            for (int mapY = 0; mapY < kMapHeight; ++mapY) {
                for (int mapX = 0; mapX < kMapWidth; ++mapX) {
                    const int cell = getCell(mapX, mapY);
                    const til::Color cellColor = cell == 0 ? til::Color{ 40, 44, 52, 255 } : scaleColor({ 180, 180, 180, 255 }, cell == 1 ? 1.0f : 0.7f);
                    framework.renderer.drawImmediatePixel(
                        window,
                        { offsetX + static_cast<til::u32>(mapX), offsetY + static_cast<til::u32>(mapY) },
                        cellColor,
                        til::BlendMode::None
                    );
                }
            }

            const til::Color playerColor { 255, 80, 80, 255 };
            framework.renderer.drawImmediatePixel(
                window,
                {
                    offsetX + static_cast<til::u32>(std::clamp(playerPos.x, 0.0f, static_cast<til::f32>(kMapWidth - 1))),
                    offsetY + static_cast<til::u32>(std::clamp(playerPos.y, 0.0f, static_cast<til::f32>(kMapHeight - 1)))
                },
                playerColor,
                til::BlendMode::None
            );

            const til::Vector2<til::f32> lookPoint {
                playerPos.x + playerDir.x * 2.0f,
                playerPos.y + playerDir.y * 2.0f
            };
            const til::Color lookColor { 255, 180, 80, 255 };
            framework.renderer.drawImmediatePixel(
                window,
                {
                    offsetX + static_cast<til::u32>(std::clamp(lookPoint.x, 0.0f, static_cast<til::f32>(kMapWidth - 1))),
                    offsetY + static_cast<til::u32>(std::clamp(lookPoint.y, 0.0f, static_cast<til::f32>(kMapHeight - 1)))
                },
                lookColor,
                til::BlendMode::None
            );
        };

        drawMinimap(player.position, player.direction);

        const til::Color crosshairColor { 255, 255, 255, 200 };
        const til::u32 centerX = size.x / 2;
        const til::u32 centerY = size.y / 2;
        if (centerX > 1 && centerY > 1) {
            framework.renderer.drawImmediatePixel(window, { centerX - 1, centerY }, crosshairColor, til::BlendMode::None);
            framework.renderer.drawImmediatePixel(window, { centerX + 1, centerY }, crosshairColor, til::BlendMode::None);
            framework.renderer.drawImmediatePixel(window, { centerX, centerY - 1 }, crosshairColor, til::BlendMode::None);
            framework.renderer.drawImmediatePixel(window, { centerX, centerY + 1 }, crosshairColor, til::BlendMode::None);
        }

        framework.display();
        framework.update();
    }

    return 0;
}
