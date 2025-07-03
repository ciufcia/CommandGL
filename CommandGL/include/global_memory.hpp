#ifndef CGL_GLOBAL_MEMORY_HPP
#define CGL_GLOBAL_MEMORY_HPP

#include <unordered_map>
#include <memory>
#include <mutex>
#include <any>

namespace cgl
{
    class GlobalMemory
    {
    public:

        static GlobalMemory& getInstance();
        
        GlobalMemory(const GlobalMemory&) = delete;
        GlobalMemory& operator=(const GlobalMemory&) = delete;

        template<typename T>
        void add(const std::string& key, const T& value);

        template<typename T>
        void set(const std::string& key, const T& value);

        template<typename T>
        const T &access(const std::string &key) const;

        template<typename T>
        T &get(const std::string &key);

        bool exists(const std::string &key) const;

        void remove(const std::string &key);

    private:

        GlobalMemory() = default;
        ~GlobalMemory() = default;

    private:

        mutable std::mutex m_mutex;

        std::unordered_map<std::string, std::any> m_storage;
    };

    template<typename T>
    void GlobalMemory::add(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage[key] = value;
    }

    template<typename T>
    void GlobalMemory::set(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_storage.find(key);
        if (it != m_storage.end()) {
            it->second = value;
        } else {
            throw std::runtime_error("Key not found in global memory");
        }
    }

    template<typename T>
    const T &GlobalMemory::access(const std::string &key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_storage.find(key);
        if (it != m_storage.end()) {
            return std::any_cast<const T&>(it->second);
        } else {
            throw std::runtime_error("Key not found in global memory");
        }
    }

    template<typename T>
    T &GlobalMemory::get(const std::string &key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_storage.find(key);
        if (it != m_storage.end()) {
            return std::any_cast<T&>(it->second);
        } else {
            throw std::runtime_error("Key not found in global memory");
        }
    }
}

#endif // CGL_GLOBAL_MEMORY_HPP