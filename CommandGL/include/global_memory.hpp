#ifndef CGL_GLOBAL_MEMORY_HPP
#define CGL_GLOBAL_MEMORY_HPP

#include <unordered_map>
#include <memory>
#include <mutex>
#include <any>

namespace cgl
{
    /**
     * @class GlobalMemory
     * @brief Thread-safe singleton for global data storage across the application.
     * 
     * The GlobalMemory class provides a centralized, thread-safe storage mechanism
     * for sharing data across different parts of the application. It uses std::any
     * for type-erased storage, allowing storage of any type of data with string keys.
     * 
     * This class implements the singleton pattern and is thread-safe through the use
     * of mutexes. All operations are atomic and can be safely called from multiple threads.
     * 
     * @note This class is non-copyable and non-assignable.
     */
    class GlobalMemory
    {
    public:

        /**
         * @brief Gets the singleton instance of GlobalMemory.
         * 
         * Returns a reference to the single instance of GlobalMemory. This method
         * is thread-safe and will create the instance on first call.
         * 
         * @return Reference to the singleton GlobalMemory instance
         * 
         * @thread_safety This method is thread-safe.
         */
        static GlobalMemory& getInstance();
        
        GlobalMemory(const GlobalMemory&) = delete;
        GlobalMemory& operator=(const GlobalMemory&) = delete;

        /**
         * @brief Adds a new key-value pair to the global memory.
         * 
         * Stores a value of type T with the given key. If the key already exists,
         * this will overwrite the existing value.
         * 
         * @tparam T The type of the value to store
         * @param key The string key to associate with the value
         * @param value The value to store
         * 
         * @thread_safety This method is thread-safe.
         * 
         * @note This method will overwrite existing values with the same key.
         */
        template<typename T>
        void add(const std::string& key, const T& value);

        /**
         * @brief Updates an existing key-value pair in the global memory.
         * 
         * Updates the value associated with the given key. Unlike add(), this method
         * will throw an exception if the key does not exist.
         * 
         * @tparam T The type of the value to store
         * @param key The string key to update
         * @param value The new value to store
         * 
         * @throws std::runtime_error if the key is not found
         * 
         * @thread_safety This method is thread-safe.
         */
        template<typename T>
        void set(const std::string& key, const T& value);

        /**
         * @brief Provides read-only access to a value in the global memory.
         * 
         * Returns a const reference to the value associated with the given key.
         * This method is preferred for read-only access as it prevents accidental
         * modification of the stored value.
         * 
         * @tparam T The type of the value to retrieve
         * @param key The string key to look up
         * @return Const reference to the stored value
         * 
         * @throws std::runtime_error if the key is not found
         * @throws std::bad_any_cast if the stored type doesn't match the requested type
         * 
         * @thread_safety This method is thread-safe.
         */
        template<typename T>
        const T &access(const std::string &key) const;

        /**
         * @brief Provides read-write access to a value in the global memory.
         * 
         * Returns a mutable reference to the value associated with the given key.
         * This allows both reading and modifying the stored value.
         * 
         * @tparam T The type of the value to retrieve
         * @param key The string key to look up
         * @return Mutable reference to the stored value
         * 
         * @throws std::runtime_error if the key is not found
         * @throws std::bad_any_cast if the stored type doesn't match the requested type
         * 
         * @thread_safety This method is thread-safe.
         */
        template<typename T>
        T &get(const std::string &key);

        /**
         * @brief Checks if a key exists in the global memory.
         * 
         * Determines whether a value is stored with the given key.
         * 
         * @param key The string key to check for existence
         * @return True if the key exists, false otherwise
         * 
         * @thread_safety This method is thread-safe.
         */
        bool exists(const std::string &key) const;

        /**
         * @brief Removes a key-value pair from the global memory.
         * 
         * Deletes the entry associated with the given key from the storage.
         * If the key does not exist, this method does nothing.
         * 
         * @param key The string key to remove
         * 
         * @thread_safety This method is thread-safe.
         */
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