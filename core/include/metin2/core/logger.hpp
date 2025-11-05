#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <mutex>
#include <fstream>
#include <chrono>
#include <iostream>

namespace metin2::core {

/**
 * Log seviyeleri
 */
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5
};

/**
 * Log sink interface - farklı çıktı destinasyonları için
 */
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(LogLevel level, const std::string& message) = 0;
    virtual void flush() = 0;
};

/**
 * Console log sink
 */
class ConsoleLogSink : public ILogSink {
public:
    void write(LogLevel level, const std::string& message) override;
    void flush() override;
};

/**
 * File log sink
 */
class FileLogSink : public ILogSink {
public:
    explicit FileLogSink(const std::string& filename);
    ~FileLogSink() override;

    void write(LogLevel level, const std::string& message) override;
    void flush() override;

private:
    std::ofstream m_file;
    std::mutex m_mutex;
};

/**
 * Android logcat sink
 */
#ifdef M2_PLATFORM_ANDROID
class AndroidLogSink : public ILogSink {
public:
    explicit AndroidLogSink(const std::string& tag);

    void write(LogLevel level, const std::string& message) override;
    void flush() override;

private:
    std::string m_tag;
};
#endif

/**
 * Logger class - Merkezi loglama sistemi
 */
class Logger {
public:
    Logger();
    ~Logger();

    // Sink ekle/çıkar
    void addSink(std::shared_ptr<ILogSink> sink);
    void removeSink(std::shared_ptr<ILogSink> sink);

    // Log seviyesi
    void setLevel(LogLevel level) { m_minLevel = level; }
    LogLevel getLevel() const { return m_minLevel; }

    // Log yazma
    void log(LogLevel level, const std::string& message);
    void log(LogLevel level, const std::string& file, int line, const std::string& message);

    // Helper metodlar
    template<typename... Args>
    void trace(Args&&... args) {
        if (m_minLevel <= LogLevel::Trace) {
            log(LogLevel::Trace, format(std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void debug(Args&&... args) {
        if (m_minLevel <= LogLevel::Debug) {
            log(LogLevel::Debug, format(std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void info(Args&&... args) {
        if (m_minLevel <= LogLevel::Info) {
            log(LogLevel::Info, format(std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void warning(Args&&... args) {
        if (m_minLevel <= LogLevel::Warning) {
            log(LogLevel::Warning, format(std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void error(Args&&... args) {
        if (m_minLevel <= LogLevel::Error) {
            log(LogLevel::Error, format(std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void critical(Args&&... args) {
        if (m_minLevel <= LogLevel::Critical) {
            log(LogLevel::Critical, format(std::forward<Args>(args)...));
        }
    }

    void flush();

private:
    // Variadic template formatting
    template<typename T>
    std::string format(T&& arg) {
        std::ostringstream oss;
        oss << std::forward<T>(arg);
        return oss.str();
    }

    template<typename T, typename... Args>
    std::string format(T&& first, Args&&... rest) {
        std::ostringstream oss;
        oss << std::forward<T>(first);
        ((oss << " " << std::forward<Args>(rest)), ...);
        return oss.str();
    }

    std::string levelToString(LogLevel level) const;
    std::string getCurrentTime() const;

    std::vector<std::shared_ptr<ILogSink>> m_sinks;
    LogLevel m_minLevel;
    std::mutex m_mutex;
};

// Global logger instance
Logger& getGlobalLogger();

} // namespace metin2::core

// Convenience macros
#define M2_LOG_TRACE(...) ::metin2::core::getGlobalLogger().trace(__VA_ARGS__)
#define M2_LOG_DEBUG(...) ::metin2::core::getGlobalLogger().debug(__VA_ARGS__)
#define M2_LOG_INFO(...) ::metin2::core::getGlobalLogger().info(__VA_ARGS__)
#define M2_LOG_WARN(...) ::metin2::core::getGlobalLogger().warning(__VA_ARGS__)
#define M2_LOG_ERROR(...) ::metin2::core::getGlobalLogger().error(__VA_ARGS__)
#define M2_LOG_CRITICAL(...) ::metin2::core::getGlobalLogger().critical(__VA_ARGS__)

// File ve line ile log
#define M2_LOG_TRACE_LOC(...) \
    ::metin2::core::getGlobalLogger().log(::metin2::core::LogLevel::Trace, __FILE__, __LINE__, \
    ::metin2::core::getGlobalLogger().format(__VA_ARGS__))

#define M2_LOG_ERROR_LOC(...) \
    ::metin2::core::getGlobalLogger().log(::metin2::core::LogLevel::Error, __FILE__, __LINE__, \
    ::metin2::core::getGlobalLogger().format(__VA_ARGS__))

/**
 * Kullanım örnekleri:
 *
 * // Initialization
 * auto& logger = getGlobalLogger();
 * logger.addSink(std::make_shared<ConsoleLogSink>());
 * logger.addSink(std::make_shared<FileLogSink>("game.log"));
 * logger.setLevel(LogLevel::Debug);
 *
 * // Usage
 * M2_LOG_INFO("Player", playerId, "connected from", ipAddress);
 * M2_LOG_ERROR("Failed to load item proto:", error);
 * M2_LOG_CRITICAL("Server crash imminent!");
 *
 * // With location
 * M2_LOG_ERROR_LOC("Invalid packet received");
 */
