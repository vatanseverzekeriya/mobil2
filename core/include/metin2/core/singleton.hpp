#pragma once

#include <memory>
#include <mutex>

namespace metin2::core {

/**
 * Thread-safe Singleton pattern
 * Modern C++ implementasyonu (Meyer's Singleton)
 */
template<typename T>
class Singleton {
public:
    // Instance'ı al
    static T& getInstance() {
        static T instance;
        return instance;
    }

    // Copy/Move engelle
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

/**
 * Lazy initialization ile Singleton
 * Manuel lifecycle kontrolü gereken durumlar için
 */
template<typename T>
class LazySingleton {
public:
    static T& getInstance() {
        std::call_once(m_initFlag, []() {
            m_instance = std::make_unique<T>();
        });
        return *m_instance;
    }

    static bool isInitialized() {
        return m_instance != nullptr;
    }

    static void destroy() {
        m_instance.reset();
    }

    LazySingleton(const LazySingleton&) = delete;
    LazySingleton& operator=(const LazySingleton&) = delete;
    LazySingleton(LazySingleton&&) = delete;
    LazySingleton& operator=(LazySingleton&&) = delete;

protected:
    LazySingleton() = default;
    virtual ~LazySingleton() = default;

private:
    static std::unique_ptr<T> m_instance;
    static std::once_flag m_initFlag;
};

template<typename T>
std::unique_ptr<T> LazySingleton<T>::m_instance = nullptr;

template<typename T>
std::once_flag LazySingleton<T>::m_initFlag;

} // namespace metin2::core

/**
 * Kullanım örneği:
 *
 * class GameConfig : public Singleton<GameConfig> {
 *     friend class Singleton<GameConfig>;
 * public:
 *     void loadConfig() { ... }
 * private:
 *     GameConfig() = default;
 * };
 *
 * // Kullanım
 * GameConfig::getInstance().loadConfig();
 */
