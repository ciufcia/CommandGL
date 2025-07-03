#include "cgl.hpp"

namespace cgl
{
    GlobalMemory& GlobalMemory::getInstance() {
        static GlobalMemory instance;
        return instance;
    }

    bool GlobalMemory::exists(const std::string &key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_storage.find(key) != m_storage.end();
    }

    void GlobalMemory::remove(const std::string &key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage.erase(key);
    }
}