#ifndef CGL_ERRORS_HPP
#define CGL_ERRORS_HPP

#include <vector>
#include <string>
#include <atomic>
#include <stdexcept>

namespace cgl
{
    class ErrorSettings
    {
    public:

        static ErrorSettings &getInstance();

        std::atomic<bool> displayErrorMessages { true };

    private:

        ErrorSettings() = default;
        ~ErrorSettings() = default;
        ErrorSettings(const ErrorSettings&) = delete;
        ErrorSettings& operator=(const ErrorSettings&) = delete;
        ErrorSettings(ErrorSettings&&) = delete;
        ErrorSettings& operator=(ErrorSettings&&) = delete;    
    };

    class Error
    {
    public:

        virtual ~Error() = default;

        void invoke(bool printMessage);

    public:

        std::string additionalDescription = "No additional description available";

    private:

        std::string constructPrettyErrorMessage();

        void printErrorMessage();
        virtual void throwException();

    protected:

        std::string constructErrorMessage();

    protected:

        std::string name = "Default Error";
        std::string description = "Base error of CommandGL";
    };

    class InvalidArgumentError : public Error
    {
    public:

        InvalidArgumentError() {
            name = "Invalid Argument Error";
            description = "Invalid argument passed";
        }

        virtual void throwException() override {
            throw std::invalid_argument(constructErrorMessage());
        }
    };

    class WinapiError : public Error
    {
    public:

        WinapiError() {
            name = "WinAPI Error";
            description = "An error occurred in the WinAPI (exclusive to Windows)";
        }
    };

    class TermiosError : public Error
    {
    public:

        TermiosError() {
            name = "Termios Error";
            description = "An error occurred in the Termios (exclusive to Unix-like systems)";
        }
    };

    class DeviceError : public Error
    {
    public:

        DeviceError() {
            name = "Device Error";
            description = "An error occurred with a device";
        }
    };

    class LogicError : public Error
    {
    public:

        LogicError() {
            name = "Logic Error";
            description = "A logic error occurred";
        }

        virtual void throwException() override {
            throw std::logic_error(constructErrorMessage());
        }
    };

    template<typename T>
    void invokeError(const std::string &additionalDescription, bool printMessage = true) {
        T error;
        error.additionalDescription = additionalDescription;
        error.invoke(printMessage);
    }

} // namespace cgl

#endif // CGL_ERRORS_HPP