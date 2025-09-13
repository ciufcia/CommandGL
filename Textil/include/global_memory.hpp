/**
 * @file global_memory.hpp
 * @brief Thread-safe global storage system for sharing data across Textil components
 * @details Provides a singleton-based global storage mechanism using std::any for
 *          type-erased storage. Enables sharing of arbitrary data between different
 *          parts of the application with thread-safe access and type-safe retrieval.
 */

#ifndef TIL_GLOBAL_MEMORY_HPP
#define TIL_GLOBAL_MEMORY_HPP

#include <unordered_map>
#include <memory>
#include <mutex>
#include <any>

namespace til
{
    /**
     * @brief Thread-safe singleton for global data storage and sharing
     * @details GlobalMemory provides a centralized, thread-safe storage system
     *          for sharing arbitrary data types across the application. It uses
     *          std::any for type-erased storage and string keys for identification.
     * 
     *          Key features:
     *          - Singleton pattern ensures single global instance
     *          - Thread-safe operations with mutex protection
     *          - Type-safe retrieval with compile-time type checking
     *          - Support for any copyable or movable type
     *          - Key-based storage similar to a global dictionary
     * 
     *          Common use cases:
     *          - Sharing configuration data between components
     *          - Caching computed results globally
     *          - Inter-component communication without tight coupling
     *          - Storing application-wide state or resources
     * 
     *          Thread safety: All operations are protected by internal mutex,
     *          making the class safe for concurrent access from multiple threads.
     */
    class GlobalMemory
    {
    public:
        /**
         * @brief Get the singleton GlobalMemory instance
         * @return Reference to the single global memory instance
         * @details Provides access to the global storage instance using
         *          thread-safe singleton pattern. First call creates the instance.
         */
        static GlobalMemory& getInstance();
        
        /**
         * @brief Deleted copy constructor to enforce singleton pattern
         */
        GlobalMemory(const GlobalMemory&) = delete;
        
        /**
         * @brief Deleted copy assignment to enforce singleton pattern
         */
        GlobalMemory& operator=(const GlobalMemory&) = delete;

        /**
         * @brief Add new key-value pair to global storage
         * @tparam T Type of value to store (must be copyable)
         * @param key String identifier for the stored value
         * @param value Value to store in global memory
         * @details Stores a copy of the value associated with the given key.
         *          If the key already exists, overwrites the previous value.
         *          Thread-safe operation.
         * 
         *          Example:
         *          ```cpp
         *          GlobalMemory::getInstance().add("window_count", 5);
         *          GlobalMemory::getInstance().add("app_name", std::string("MyApp"));
         *          ```
         */
        template<typename T>
        void add(const std::string& key, const T& value);
        
        /**
         * @brief Update existing key with new value
         * @tparam T Type of value to store
         * @param key String identifier of existing key
         * @param value New value to store
         * @throws InvalidArgumentError if key does not exist
         * @details Updates the value for an existing key. Unlike add(),
         *          this method requires the key to already exist and will
         *          throw an error if it doesn't. Thread-safe operation.
         */
        template<typename T>
        void set(const std::string& key, const T& value);
        
        /**
         * @brief Get read-only access to stored value
         * @tparam T Expected type of the stored value
         * @param key String identifier of the value
         * @return Const reference to the stored value
         * @throws InvalidArgumentError if key does not exist
         * @throws std::bad_any_cast if stored type doesn't match T
         * @details Provides read-only access to stored values with compile-time
         *          type safety. The type T must match the originally stored type.
         *          Thread-safe operation.
         */
        template<typename T>
        const T &access(const std::string &key) const;
        
        /**
         * @brief Get mutable access to stored value
         * @tparam T Expected type of the stored value
         * @param key String identifier of the value
         * @return Mutable reference to the stored value
         * @throws InvalidArgumentError if key does not exist
         * @throws std::bad_any_cast if stored type doesn't match T
         * @details Provides mutable access to stored values allowing in-place
         *          modification. The type T must match the originally stored type.
         *          Thread-safe operation.
         */
        template<typename T>
        T &get(const std::string &key);
        
        /**
         * @brief Check if key exists in storage
         * @param key String identifier to check
         * @return True if key exists, false otherwise
         * @details Non-throwing method to check key existence before access.
         *          Useful for conditional access patterns. Thread-safe operation.
         */
        bool exists(const std::string &key) const;
        
        /**
         * @brief Remove key-value pair from storage
         * @param key String identifier of pair to remove
         * @details Removes the specified key and its associated value from storage.
         *          No error if key doesn't exist. Thread-safe operation.
         */
        void remove(const std::string &key);

    private:
        /**
         * @brief Private constructor for singleton pattern
         */
        GlobalMemory() = default;
        
        /**
         * @brief Private destructor for singleton pattern
         */
        ~GlobalMemory() = default;

    private:
        mutable std::mutex m_mutex; ///< Mutex for thread-safe access to storage
        std::unordered_map<std::string, std::any> m_storage; ///< Key-value storage using type erasure
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
            invokeError<InvalidArgumentError>("Key not found in global memory");
        }
    }

    template<typename T>
    const T &GlobalMemory::access(const std::string &key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_storage.find(key);
        if (it != m_storage.end()) {
            return std::any_cast<const T&>(it->second);
        } else {
            invokeError<InvalidArgumentError>("Key not found in global memory");
        }
    }

    template<typename T>
    T &GlobalMemory::get(const std::string &key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_storage.find(key);
        if (it != m_storage.end()) {
            return std::any_cast<T&>(it->second);
        } else {
            invokeError<InvalidArgumentError>("Key not found in global memory");
        }
    }
}

#endif // TIL_GLOBAL_MEMORY_HPP