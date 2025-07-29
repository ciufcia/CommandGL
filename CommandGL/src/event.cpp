#include "cgl.hpp"

namespace cgl
{
int getWinapiVK(KeyCode key) {
    auto it = keyCodeToWinapiVK.find(key);
    if (it != keyCodeToWinapiVK.end()) {
        return static_cast<int>(it->second);
    }
    return -1;
}
}