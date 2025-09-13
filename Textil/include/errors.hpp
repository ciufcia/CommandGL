/**
 * @file errors.hpp
 * @brief Error handling system for Textil library
 * 
 * @details Provides a comprehensive error handling framework with configurable error reporting,
 * multiple error types for different subsystems, and unified error processing.
 * The system supports both exception throwing and graceful error handling patterns.
 */

#ifndef TIL_ERRORS_HPP
#define TIL_ERRORS_HPP

#include <vector>
#include <string>
#include <atomic>
#include <stdexcept>

namespace til
{
    /**
     * @brief Singleton class managing global error handling settings
     * @details Provides thread-safe configuration for error display and handling behavior.
     *          Uses atomic operations to ensure thread safety in multi-threaded environments.
     *          Applications can use this to control error verbosity and display preferences.
     */
    class ErrorSettings
    {
    public:
        /**
         * @brief Get the singleton instance
         * @return Reference to the global ErrorSettings instance
         * @details Thread-safe singleton implementation providing global access to error settings.
         */
        static ErrorSettings &getInstance();

        /// Atomic flag controlling whether error messages are displayed to console
        /// Set to false to suppress error output for release builds or quiet operation
        std::atomic<bool> displayErrorMessages { true };

    private:
        ErrorSettings() = default;
        ~ErrorSettings() = default;
        ErrorSettings(const ErrorSettings&) = delete;
        ErrorSettings& operator=(const ErrorSettings&) = delete;
        ErrorSettings(ErrorSettings&&) = delete;
        ErrorSettings& operator=(ErrorSettings&&) = delete;    
    };

    /**
     * @brief Base error class for all Textil error types
     * @details Abstract base class providing common error handling functionality.
     *          Supports both exception throwing and non-throwing error reporting.
     *          Derived classes implement specific error types with appropriate exception types.
     */
    class Error
    {
    public:
        /**
         * @brief Virtual destructor for proper inheritance
         */
        virtual ~Error() = default;

        /**
         * @brief Invoke error handling with optional message display
         * @param printMessage Whether to display error message to console
         * @details Main error processing function that handles message display and exception throwing.
         *          Respects global ErrorSettings for message display control.
         */
        void invoke(bool printMessage);

    public:
        /// Additional context-specific description for the error condition
        /// Can be set by calling code to provide more detailed error information
        std::string additionalDescription = "No additional description available";

    private:
        /**
         * @brief Construct formatted error message for display
         * @return Formatted error message string
         * @details Creates user-friendly error message combining error name, description, and additional context.
         */
        std::string constructPrettyErrorMessage();

        /**
         * @brief Print error message to console
         * @details Outputs formatted error message to standard error stream.
         */
        void printErrorMessage();
        
        /**
         * @brief Throw appropriate exception type (virtual)
         * @details Pure virtual method implemented by derived classes to throw specific exception types.
         *          Base implementation does not throw to allow for graceful error handling.
         */
        virtual void throwException();

    protected:
        /**
         * @brief Construct basic error message string
         * @return Error message combining name, description, and additional context
         * @details Protected helper for creating error messages used by both display and exception systems.
         */
        std::string constructErrorMessage();

    protected:
        std::string name = "Default Error";                ///< Human-readable error type name
        std::string description = "Base error of Textil";  ///< General description of error condition
    };

    /**
     * @brief Error for invalid function arguments or parameters
     * @details Thrown when functions receive parameters that are out of range, null when non-null expected,
     *          or otherwise violate function preconditions. Maps to std::invalid_argument exception.
     */
    class InvalidArgumentError : public Error
    {
    public:
        /**
         * @brief Constructor setting appropriate error identification
         */
        InvalidArgumentError() {
            name = "Invalid Argument Error";
            description = "Invalid argument passed";
        }

        /**
         * @brief Throw std::invalid_argument exception
         * @throws std::invalid_argument with constructed error message
         */
        virtual void throwException() override {
            throw std::invalid_argument(constructErrorMessage());
        }
    };

    /**
     * @brief Error for Windows API function failures
     * @details Specific to Windows platform operations. Used when WinAPI functions fail or return error codes.
     *          Helps isolate platform-specific error handling in cross-platform code.
     */
    class WinapiError : public Error
    {
    public:
        /**
         * @brief Constructor setting Windows-specific error identification
         */
        WinapiError() {
            name = "WinAPI Error";
            description = "An error occurred in the WinAPI (exclusive to Windows)";
        }
    };

    /**
     * @brief Error for Unix/Linux terminal interface failures
     * @details Specific to Unix-like platform terminal operations. Used when termios functions fail
     *          or terminal configuration encounters problems. Platform-specific complement to WinapiError.
     */
    class TermiosError : public Error
    {
    public:
        /**
         * @brief Constructor setting Unix-specific error identification
         */
        TermiosError() {
            name = "Termios Error";
            description = "An error occurred in the Termios (exclusive to Unix-like systems)";
        }
    };

    /**
     * @brief Error for hardware device access failures
     * @details Used when input/output devices (keyboard, mouse, etc.) cannot be accessed or configured.
     *          Common causes include permission issues, device disconnection, or driver problems.
     */
    class DeviceError : public Error
    {
    public:
        /**
         * @brief Constructor setting device-specific error identification
         */
        DeviceError() {
            name = "Device Error";
            description = "An error occurred with a device";
        }
    };

    /**
     * @brief Error for logic and programming errors
     * @details Indicates programming errors, invalid state conditions, or violation of logical preconditions.
     *          Usually indicates bugs in calling code or improper API usage. Maps to std::logic_error exception.
     */
    class LogicError : public Error
    {
    public:
        /**
         * @brief Constructor setting logic-specific error identification
         */
        LogicError() {
            name = "Logic Error";
            description = "A logic error occurred";
        }

        /**
         * @brief Throw std::logic_error exception
         * @throws std::logic_error with constructed error message
         */
        virtual void throwException() override {
            throw std::logic_error(constructErrorMessage());
        }
    };

    /**
     * @brief Convenience function for creating and invoking specific error types
     * @tparam T Error class type to instantiate (must derive from Error)
     * @param additionalDescription Context-specific error description
     * @param printMessage Whether to display error message (defaults to true)
     * @details Template function providing convenient error invocation with automatic type deduction.
     *          Usage: invokeError<InvalidArgumentError>("File not found", true);
     */
    template<typename T>
    void invokeError(const std::string &additionalDescription, bool printMessage = true) {
        T error;
        error.additionalDescription = additionalDescription;
        error.invoke(printMessage);
    }

} // namespace til

#endif // TIL_ERRORS_HPP