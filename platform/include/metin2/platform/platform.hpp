#pragma once

#include <string>
#include <cstdint>

/**
 * Platform detection macros
 */
#if defined(__ANDROID__)
    #define M2_PLATFORM_ANDROID
    #define M2_MOBILE
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define M2_PLATFORM_IOS
        #define M2_MOBILE
    #elif TARGET_OS_MAC
        #define M2_PLATFORM_MACOS
    #endif
#elif defined(_WIN32) || defined(_WIN64)
    #define M2_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define M2_PLATFORM_LINUX
#endif

/**
 * Architecture detection
 */
#if defined(__aarch64__) || defined(_M_ARM64)
    #define M2_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define M2_ARCH_ARM32
#elif defined(__x86_64__) || defined(_M_X64)
    #define M2_ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
    #define M2_ARCH_X86
#endif

/**
 * Compiler detection
 */
#if defined(__clang__)
    #define M2_COMPILER_CLANG
#elif defined(__GNUC__)
    #define M2_COMPILER_GCC
#elif defined(_MSC_VER)
    #define M2_COMPILER_MSVC
#endif

namespace metin2::platform {

/**
 * Platform bilgileri
 */
enum class PlatformType {
    Unknown,
    Windows,
    Linux,
    MacOS,
    Android,
    iOS
};

enum class Architecture {
    Unknown,
    x86,
    x64,
    ARM32,
    ARM64
};

/**
 * Platform info
 */
struct PlatformInfo {
    PlatformType platform;
    Architecture architecture;
    std::string osVersion;
    std::string deviceModel;
    uint32_t cpuCores;
    uint64_t totalMemory;
    uint64_t availableMemory;
};

/**
 * Platform interface
 */
class IPlatform {
public:
    virtual ~IPlatform() = default;

    // Platform bilgisi
    virtual PlatformInfo getPlatformInfo() const = 0;
    virtual std::string getPlatformName() const = 0;

    // Dosya sistemi
    virtual std::string getDataPath() const = 0;
    virtual std::string getCachePath() const = 0;
    virtual std::string getTempPath() const = 0;
    virtual std::string getDocumentsPath() const = 0;

    // Sistem
    virtual void vibrate(uint32_t durationMs) = 0;
    virtual void openURL(const std::string& url) = 0;
    virtual void showKeyboard(bool show) = 0;

    // Battery (mobile)
    virtual float getBatteryLevel() const = 0;
    virtual bool isBatteryCharging() const = 0;

    // Network
    virtual bool isNetworkAvailable() const = 0;
    virtual bool isWiFiConnected() const = 0;

    // Performance
    virtual void setLowPowerMode(bool enable) = 0;
    virtual bool isLowPowerMode() const = 0;

    // Screen
    virtual void getScreenSize(uint32_t& width, uint32_t& height) const = 0;
    virtual float getScreenDPI() const = 0;
    virtual void setScreenOrientation(bool landscape) = 0;
};

/**
 * Platform factory
 */
class PlatformFactory {
public:
    static IPlatform* create();
    static void destroy(IPlatform* platform);
};

/**
 * Global platform instance
 */
IPlatform& getPlatform();

} // namespace metin2::platform

/**
 * Platform-specific macros
 */

// DLL export/import
#ifdef M2_PLATFORM_WINDOWS
    #ifdef M2_BUILD_DLL
        #define M2_API __declspec(dllexport)
    #else
        #define M2_API __declspec(dllimport)
    #endif
#else
    #define M2_API __attribute__((visibility("default")))
#endif

// Force inline
#ifdef M2_COMPILER_MSVC
    #define M2_FORCE_INLINE __forceinline
#else
    #define M2_FORCE_INLINE inline __attribute__((always_inline))
#endif

// Debug break
#ifdef M2_COMPILER_MSVC
    #define M2_DEBUG_BREAK() __debugbreak()
#else
    #define M2_DEBUG_BREAK() __builtin_trap()
#endif

// Thread local
#define M2_THREAD_LOCAL thread_local

/**
 * Kullanım örneği:
 *
 * auto& platform = getPlatform();
 * auto info = platform.getPlatformInfo();
 *
 * M2_LOG_INFO("Platform:", platform.getPlatformName());
 * M2_LOG_INFO("CPU Cores:", info.cpuCores);
 * M2_LOG_INFO("Memory:", info.totalMemory / (1024 * 1024), "MB");
 *
 * #ifdef M2_MOBILE
 *     platform.vibrate(100);
 *     float battery = platform.getBatteryLevel();
 *     M2_LOG_INFO("Battery:", battery * 100, "%");
 * #endif
 */
