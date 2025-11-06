/**
 * @file transform.hpp
 * @brief 2D transformation system for Textil library
 * @details Provides a comprehensive transformation class for 2D graphics operations
 *          including translation, rotation, scaling, and origin-based transformations.
 *          Integrates with Matrix3 for efficient transformation composition and application.
 */

#ifndef TIL_TRANSFORM_HPP
#define TIL_TRANSFORM_HPP

#include "matrix3.hpp"
#include "vector2.hpp"
#include "numeric_types.hpp"
#include <cmath>
#include <numbers>

namespace til
{
    /**
     * @brief 2D transformation class for position, rotation, and scale operations
     * @details Encapsulates common 2D transformation properties and provides convenient
     *          methods for manipulation. Internally converts to Matrix3 for efficient
     *          application to geometry. Supports hierarchical transformations and
     *          origin-based operations for intuitive object manipulation.
     * 
    *          The transformation order is: Scale → Rotate → Translate, applied relative
    *          to the specified origin point. This order ensures predictable behavior
    *          for most graphics operations.
    * 
    *          Coordinate system conventions:
    *          - Positive X points right
    *          - Positive Y points down (screen coordinates)
    *          - Rotation is stored internally in radians (positive values rotate clockwise)
    *            but `setRotation()` expects degrees for convenience
    *          - Origin (0,0) is typically top-left corner
     */
    class Transform
    {
    public:
        /**
         * @brief Default constructor creating identity transformation
         * @details Creates transformation with position (0,0), scale (1,1), rotation 0,
         *          and origin (0,0). This represents no transformation - objects remain
         *          in their original position and orientation.
         */
        Transform();
        
        /**
         * @brief Constructor with explicit transformation parameters
         * @param position Translation offset in world coordinates
         * @param scale Scaling factors for X and Y axes (1.0 = no scaling)
         * @param rotation Rotation angle in radians (positive = clockwise)
         * @param origin Point around which rotation and scaling occur
         * @details Creates transformation with all parameters specified explicitly.
         *          The origin parameter defines the center point for rotation and scaling,
         *          allowing intuitive manipulation of objects around their centers.
         */
        Transform(const Vector2<f32>& position, const Vector2<f32>& scale, f32 rotation, const Vector2<f32>& origin);

        /**
         * @brief Generate transformation matrix for graphics operations
         * @return 3x3 transformation matrix representing this transform
         * @details Converts the transformation parameters into a Matrix3 suitable for
         *          applying to points and vectors. The matrix encodes the complete
         *          transformation including origin offset, scaling, rotation, and translation.
         *          
         *          Matrix composition order: T * R * S * -O
         *          Where: T=translation, R=rotation, S=scale, O=origin offset
         */
        Matrix3<f32> getMatrix() const;

        /**
         * @brief Get current position/translation
         * @return Position vector in world coordinates
         */
        Vector2<f32> getPosition() const;
        
        /**
         * @brief Get current scale factors
         * @return Scale vector with X and Y scaling factors
         */
        Vector2<f32> getScale() const;
        
    /**
     * @brief Get current rotation angle
     * @return Rotation in radians (positive = clockwise)
     */
        f32 getRotation() const;
        
        /**
         * @brief Get current origin point
         * @return Origin vector for rotation and scaling center
         */
        Vector2<f32> getOrigin() const;

        /**
         * @brief Set absolute position in world coordinates
         * @param position New position vector
         * @details Sets the translation component, determining where the transformed
         *          object appears in world space after all other transformations.
         */
        void setPosition(const Vector2<f32>& position);
        
        /**
         * @brief Set scaling factors for both axes
         * @param scale New scale vector (1.0 = original size)
         * @details Controls object size scaling. Values > 1.0 enlarge, < 1.0 shrink.
         *          Negative values flip the object along the corresponding axis.
         *          Scaling occurs around the origin point.
         */
        void setScale(const Vector2<f32>& scale);
        
    /**
     * @brief Set absolute rotation angle
     * @param rotation New rotation in degrees (positive = clockwise)
     * @details Sets object orientation. Rotation occurs around the origin point.
     *          The value is converted to radians internally. Full rotation is 360 degrees.
     */
        void setRotation(f32 rotation);
        
        /**
         * @brief Set origin point for rotation and scaling
         * @param origin New origin point in local object coordinates
         * @details Defines the center point for rotation and scaling operations.
         *          Typically set to object center for intuitive transformations.
         *          Origin (0,0) means rotation/scaling occurs around top-left corner.
         */
        void setOrigin(const Vector2<f32>& origin);
        
        /**
         * @brief Apply relative rotation to current rotation
         * @param degrees Rotation to add in degrees (positive = clockwise)
         * @details Increments the current rotation by the specified amount.
         *          Convenient for continuous rotation animations or user input.
         *          Internally converts degrees to radians before applying.
         */
        void rotate(f32 degrees);
        
        /**
         * @brief Apply relative translation to current position
         * @param delta Translation vector to add to current position
         * @details Moves the object by the specified offset relative to its current position.
         *          Equivalent to: setPosition(getPosition() + delta)
         */
        void move(const Vector2<f32>& delta);

        /**
         * @brief Convert degrees to radians
         * @param degrees Angle in degrees
         * @return Equivalent angle in radians
         * @details Utility function for angle conversion: radians = degrees * π / 180
         *          Useful when working with degree-based input but needing radians internally.
         */
        static float degreesToRadians(float degrees);
        
        /**
         * @brief Convert radians to degrees  
         * @param radians Angle in radians
         * @return Equivalent angle in degrees
         * @details Utility function for angle conversion: degrees = radians * 180 / π
         *          Useful for displaying angles in user-friendly degree format.
         */
        static float radiansToDegrees(float radians);

    private:
        Vector2<f32> m_position { 0.f, 0.f }; ///< Object position in world coordinates
        Vector2<f32> m_scale { 1.f, 1.f };    ///< Scaling factors for X and Y axes
        f32          m_rotation { 0.f };      ///< Rotation angle in radians
        Vector2<f32> m_origin { 0.f, 0.f };   ///< Origin point for rotation and scaling

    friend class Framework; ///< Framework needs access for internal operations
    };
}

#endif // TIL_TRANSFORM_HPP