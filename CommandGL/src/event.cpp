#include "cgl.hpp"

namespace cgl
{
#ifdef _WIN32
int getWinapiVK(KeyCode key) {
    auto it = keyCodeToWinapiVK.find(key);
    if (it != keyCodeToWinapiVK.end()) {
        return static_cast<int>(it->second);
    }
    return -1;
}
#endif // _WIN32

#ifdef __linux__
int getLinuxKey(KeyCode key) {
    auto it = keyCodeToLinuxKey.find(key);
    if (it != keyCodeToLinuxKey.end()) {
        return it->second;
    }
    return -1;
}

KeyCode getKeyCodeFromLinuxKey(int key) {
    for (const auto &pair : keyCodeToLinuxKey) {
        if (pair.second == key) {
            return pair.first;
        }
    }
    return KeyCode::Invalid;
}
#endif // __linux__
}