#pragma once

#include "packet.hpp"
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>

// Platform-specific networking
#ifdef M2_PLATFORM_WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

namespace metin2::network {

/**
 * Session durumları
 */
enum class SessionState {
    Disconnected,
    Connecting,
    Connected,
    Authenticated,
    InGame
};

/**
 * Paket handler callback
 */
using PacketHandler = std::function<void(const uint8_t* data, size_t size)>;

/**
 * Network session - Client veya server connection
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(SOCKET socket);
    ~Session();

    // Connection yönetimi
    bool connect(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const;

    // Paket gönderme
    bool sendPacket(const void* data, size_t size);
    bool sendPacket(const PacketBuffer& buffer);

    template<typename T>
    bool sendPacket(const T& packet) {
        static_assert(std::is_trivially_copyable_v<T>,
                     "Packet must be trivially copyable");
        return sendPacket(&packet, sizeof(T));
    }

    // Paket alma (non-blocking)
    bool receivePackets();

    // Paket handler kaydetme
    void setPacketHandler(uint8_t packetId, PacketHandler handler);
    void setDefaultHandler(PacketHandler handler);

    // Session bilgileri
    uint32_t getSessionId() const { return m_sessionId; }
    SessionState getState() const { return m_state; }
    void setState(SessionState state) { m_state = state; }

    // Player bilgisi (authenticated session için)
    void setPlayerId(uint32_t playerId) { m_playerId = playerId; }
    uint32_t getPlayerId() const { return m_playerId; }

    // Statistics
    size_t getBytesSent() const { return m_bytesSent; }
    size_t getBytesReceived() const { return m_bytesReceived; }

    // Latency tracking (mobile için önemli)
    void updatePing(uint32_t ping) { m_ping = ping; }
    uint32_t getPing() const { return m_ping; }

private:
    void processPacket(const uint8_t* data, size_t size);

    SOCKET m_socket;
    uint32_t m_sessionId;
    std::atomic<SessionState> m_state;
    uint32_t m_playerId;

    // Receive buffer
    std::vector<uint8_t> m_recvBuffer;
    size_t m_recvBufferPos;

    // Send queue (thread-safe)
    std::queue<std::vector<uint8_t>> m_sendQueue;
    std::mutex m_sendMutex;

    // Packet handlers
    std::unordered_map<uint8_t, PacketHandler> m_handlers;
    PacketHandler m_defaultHandler;

    // Statistics
    std::atomic<size_t> m_bytesSent;
    std::atomic<size_t> m_bytesReceived;
    std::atomic<uint32_t> m_ping;

    static std::atomic<uint32_t> s_nextSessionId;
};

/**
 * Session Manager - Tüm aktif session'ları yönetir
 */
class SessionManager {
public:
    SessionManager() = default;
    ~SessionManager() = default;

    // Session ekleme/çıkarma
    void addSession(std::shared_ptr<Session> session);
    void removeSession(uint32_t sessionId);
    std::shared_ptr<Session> getSession(uint32_t sessionId);

    // Broadcast (tüm session'lara gönder)
    void broadcast(const void* data, size_t size);

    template<typename T>
    void broadcast(const T& packet) {
        static_assert(std::is_trivially_copyable_v<T>,
                     "Packet must be trivially copyable");
        broadcast(&packet, sizeof(T));
    }

    // Tüm session'ları güncelle
    void update();

    // Aktif session sayısı
    size_t getSessionCount() const;

    // Cleanup (disconnected sessions)
    void cleanup();

private:
    std::unordered_map<uint32_t, std::shared_ptr<Session>> m_sessions;
    std::mutex m_mutex;
};

} // namespace metin2::network

/**
 * Kullanım örneği (Client):
 *
 * auto session = std::make_shared<Session>(INVALID_SOCKET);
 *
 * // Connect
 * if (!session->connect("127.0.0.1", 13000)) {
 *     M2_LOG_ERROR("Connection failed");
 *     return;
 * }
 *
 * // Register packet handler
 * session->setPacketHandler(ServerPackets::LOGIN_SUCCESS, [](const uint8_t* data, size_t size) {
 *     M2_LOG_INFO("Login successful!");
 * });
 *
 * // Send packet
 * LoginPacket loginPkt;
 * // ... fill packet
 * session->sendPacket(loginPkt);
 *
 * // Game loop
 * while (running) {
 *     session->receivePackets();
 *     // ...
 * }
 *
 * // Disconnect
 * session->disconnect();
 */
