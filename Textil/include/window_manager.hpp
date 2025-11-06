/**
 * @file window_manager.hpp
 * @brief Window management system for organizing multiple rendering contexts
 * @details Provides centralized management of windows including creation, destruction,
 *          depth sorting, and coordinated rendering. Handles window layering and
 *          ensures proper rendering order for overlapping windows.
 */

#ifndef TIL_WINDOW_MANAGER_HPP
#define TIL_WINDOW_MANAGER_HPP

#include "numeric_types.hpp"
#include "window.hpp"
#include <vector>

namespace til
{
    /**
     * @brief Centralized manager for window creation, organization, and rendering
     * @details The WindowManager class coordinates multiple Window instances, providing:
     *          - Window lifecycle management (creation/destruction)
     *          - Unique ID assignment and tracking
     *          - Depth-based rendering order
     *          - Coordinated rendering pipeline execution
     *          - Character buffer management for terminal output
     * 
    *          Windows are stored in a list and can be accessed by their unique IDs.
    *          The manager ensures proper layering by sorting windows by depth before
    *          rendering. Lower depth values are drawn last, making them appear on top.
     * 
     *          This class is typically used by the Framework and not directly by
     *          applications, though it provides the public interface for window operations.
     */
    class WindowManager
    {
    public:
        /**
         * @brief Default constructor
         * @details Initializes an empty window manager ready to create windows.
         */
        WindowManager() = default;

        /**
         * @brief Deleted copy constructor to prevent copying
         * @details WindowManager manages unique window resources and cannot be safely copied.
         */
        WindowManager(const WindowManager&) = delete;
        
        /**
         * @brief Deleted copy assignment operator
         * @details WindowManager manages unique window resources and cannot be safely copied.
         */
        WindowManager& operator=(const WindowManager&) = delete;

        /**
         * @brief Create a new window with automatic ID assignment
         * @return Reference to the newly created window
         * @details Creates a new Window instance with a unique ID and adds it to
         *          the managed window list. The window is initialized with default
         *          values and can be customized after creation.
         * 
         *          Example usage:
         *          ```cpp
         *          Window& window = windowManager.createWindow();
         *          window.setPosition({10, 5});
         *          window.setSize({80, 24});
         *          ```
         */
        Window &createWindow();
        
    /**
     * @brief Destroy window with specified ID
     * @param id Unique identifier of window to destroy
     * @details Removes the window from management and releases its resources.
     *          After destruction, the window ID becomes available for reuse.
     * @throws InvalidArgumentError if the ID does not reference a managed window
     */
        void destroyWindow(u32 id);
        
    /**
     * @brief Get reference to window by ID
     * @param id Unique identifier of window to retrieve
     * @return Reference to the specified window
     * @throws InvalidArgumentError if the ID is not found
     * @details Provides access to existing windows for modification or rendering.
     *          The returned reference remains valid until the window is destroyed.
     */
        Window &getWindow(u32 id);

        /**
         * @brief Get read-only access to all managed windows
         * @return Const reference to internal window list
         * @details Provides iteration access to all windows for inspection or
         *          read-only operations. Windows retain the ordering established
         *          by the most recent call to sortByDepth(), so the list typically
         *          reflects current rendering order (highest depth first).
         */
        const std::list<Window>& getWindows() const;

    private:
        /**
         * @brief Execute rendering pipeline for all windows
         * @details Internal method that processes all windows through their
         *          rendering pipelines. Called by Framework during display cycle.
         */
        void renderWindows();

    /**
     * @brief Sort windows by depth for proper layering
     * @details Arranges windows so that lower depth values are processed last,
     *          allowing them to appear in front of higher depth windows. Called
     *          before rendering to ensure consistent overlap behaviour.
     */
        void sortByDepth();

        /**
         * @brief Generate unique ID for new window
         * @return Next available window ID
         * @details Internal method for ID management, ensuring each window
         *          receives a unique identifier for lifetime tracking.
         */
        u32 getNextId();

        /**
         * @brief Get mutable access to window list
         * @return Reference to internal window list for modification
         * @details Internal accessor for window list manipulation.
         */
        std::list<Window>& getWindows();

    private:
        std::list<Window> m_windows {}; ///< Container storing all managed windows
        u32 m_nextId = 1; ///< Next ID to assign to new windows (0 reserved for invalid)
        std::set<u32> m_usedIds; ///< Set tracking currently assigned window IDs

        /// Shared character buffer for compositing window output
        std::vector<CharacterCell> m_characterBuffer {};

    friend class Framework; ///< Framework needs access for rendering coordination
    };
}

#endif // TIL_WINDOW_MANAGER_HPP