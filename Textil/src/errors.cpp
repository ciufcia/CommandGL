#include "til.hpp"
#include <sstream>
#include <iostream>

namespace {
    constexpr const char *CGL_COL_RESET = "\x1b[0m";
    constexpr const char *CGL_COL_RED   = "\x1b[31m";
    constexpr const char *CGL_STYLE_BOLD = "\x1b[1m";
}

namespace til {
    void Error::invoke(bool printMessage) {
        if (printMessage) {
            if (ErrorSettings::getInstance().displayErrorMessages) {
                printErrorMessage();
                std::cout << "Press Any key to quit...";
                std::cin.get();
            }
        }

        throwException();
    }

    ErrorSettings& ErrorSettings::getInstance() {
        static ErrorSettings instance;
        return instance;
    }

    std::string Error::constructErrorMessage() {
        std::string msg;
        if(!name.empty()) {
            msg += name;
        }
        if(!description.empty()) {
            if(!msg.empty()) msg += ": ";
            msg += description;
        }
        if(!additionalDescription.empty()) {
            if(!msg.empty()) msg += '\n';
            msg += additionalDescription;
        }
        return msg;
    }

    std::string Error::constructPrettyErrorMessage() {
        constexpr size_t MAX_WIDTH = 80;

        std::vector<std::string> logicalLines;
        std::string header = "[ERROR]";
        if(!name.empty()) header += std::string(" ") + name;
        logicalLines.push_back(header);
        if(!description.empty()) logicalLines.push_back(description);
        if(!additionalDescription.empty()) {
            size_t start = 0;
            while (start <= additionalDescription.size()) {
                size_t nl = additionalDescription.find('\n', start);
                if (nl == std::string::npos) { logicalLines.push_back(additionalDescription.substr(start)); break; }
                logicalLines.push_back(additionalDescription.substr(start, nl - start));
                start = nl + 1;
            }
        }

        std::vector<std::string> wrapped;
        auto wrap = [&](const std::string &line){
            if(line.empty()) { wrapped.emplace_back(""); return; }
            size_t pos = 0; std::string current;
            while(pos < line.size()) {
                while(pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])) && line[pos] != '\n') pos++;
                size_t ws = pos;
                while(pos < line.size() && !std::isspace(static_cast<unsigned char>(line[pos]))) pos++;
                std::string word = line.substr(ws, pos - ws);
                if(word.empty()) break;
                if(current.empty()) {
                    if(word.size() <= MAX_WIDTH) {
                        current = word;
                    } else {
                        size_t idx = 0; while(idx < word.size()) { size_t chunk = std::min(MAX_WIDTH, word.size()-idx); wrapped.emplace_back(word.substr(idx, chunk)); idx += chunk; }
                        current.clear();
                    }
                } else {
                    if(current.size() + 1 + word.size() <= MAX_WIDTH) { current += ' '; current += word; }
                    else { wrapped.push_back(current); current = word; }
                }
                while(pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])) && line[pos] != '\n') pos++;
            }
            if(!current.empty()) wrapped.push_back(current);
        };
        for(auto &l : logicalLines) wrap(l);
        if(wrapped.empty()) wrapped.emplace_back("");

        size_t width = 0; for(auto &l : wrapped) width = std::max(width, l.size());
        if(width > MAX_WIDTH) width = MAX_WIDTH; // safety

        auto pad = [&](const std::string &l){ return l + std::string(width - std::min(width, l.size()), ' '); };

        std::ostringstream oss;
        std::string frame = "+-" + std::string(width, '-') + "-+";
        oss << CGL_STYLE_BOLD << CGL_COL_RED << frame << CGL_COL_RESET << '\n';

        bool headerDone = false; bool descStarted = false; bool haveDesc = !description.empty();
        for(size_t i = 0; i < wrapped.size(); ++i) {
            const std::string &raw = wrapped[i];
            std::string padded = pad(raw.substr(0, width));
            bool isHeader = !headerDone;
            if(isHeader) headerDone = true;
            bool isDesc = !isHeader && haveDesc && !descStarted; if(isDesc) descStarted = true;
            oss << CGL_COL_RED << '|' << CGL_COL_RESET << ' ';
            if(isHeader) {
                size_t tokenPos = padded.find("[ERROR]");
                if(tokenPos != std::string::npos) {
                    std::string before = padded.substr(0, tokenPos);
                    std::string token = padded.substr(tokenPos, 7);
                    std::string after = padded.substr(tokenPos + 7);
                    oss << before << CGL_STYLE_BOLD << CGL_COL_RED << token << CGL_COL_RESET;
                    if(!after.empty()) oss << CGL_STYLE_BOLD << after << CGL_COL_RESET;
                } else {
                    oss << CGL_STYLE_BOLD << CGL_COL_RED << padded << CGL_COL_RESET;
                }
            } else if(isDesc) {
                oss << CGL_COL_RED << padded << CGL_COL_RESET;
            } else {
                oss << padded;
            }
            oss << ' ' << CGL_COL_RED << '|' << CGL_COL_RESET << '\n';
        }

        oss << CGL_STYLE_BOLD << CGL_COL_RED << frame << CGL_COL_RESET << '\n';
        return oss.str();
    }

    void Error::printErrorMessage() {
#if defined(_WIN32)
    std::system("cls");
#elif defined(__APPLE__) || defined(__linux__)
    std::system("clear");
#endif

        std::cout << constructPrettyErrorMessage() << std::endl;
    }

    void Error::throwException() {
        throw std::runtime_error(constructErrorMessage());
    }
} // namespace til