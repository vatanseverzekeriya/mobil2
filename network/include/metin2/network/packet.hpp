#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <memory>

namespace metin2::network {

/**
 * Paket header yapısı
 */
#pragma pack(push, 1)
struct PacketHeader {
    uint8_t id;           // Paket tipi
    uint16_t size;        // Toplam paket boyutu (header dahil)

    PacketHeader() : id(0), size(0) {}
    PacketHeader(uint8_t id_, uint16_t size_) : id(id_), size(size_) {}
};
#pragma pack(pop)

/**
 * Paket tipleri (Client -> Server)
 */
namespace ClientPackets {
    constexpr uint8_t LOGIN = 1;
    constexpr uint8_t SELECT_CHARACTER = 2;
    constexpr uint8_t MOVE = 3;
    constexpr uint8_t ATTACK = 4;
    constexpr uint8_t CHAT = 5;
    constexpr uint8_t ITEM_PICK = 6;
    constexpr uint8_t ITEM_USE = 7;
    constexpr uint8_t ITEM_DROP = 8;
    constexpr uint8_t ITEM_MOVE = 9;
    constexpr uint8_t SKILL_USE = 10;
    constexpr uint8_t SHOP_BUY = 11;
    constexpr uint8_t SHOP_SELL = 12;
    constexpr uint8_t QUEST_ANSWER = 13;
}

/**
 * Paket tipleri (Server -> Client)
 */
namespace ServerPackets {
    constexpr uint8_t LOGIN_SUCCESS = 101;
    constexpr uint8_t LOGIN_FAILURE = 102;
    constexpr uint8_t CHARACTER_INFO = 103;
    constexpr uint8_t SPAWN_CHARACTER = 104;
    constexpr uint8_t DESPAWN_CHARACTER = 105;
    constexpr uint8_t MOVE = 106;
    constexpr uint8_t CHAT = 107;
    constexpr uint8_t DAMAGE = 108;
    constexpr uint8_t ITEM_ADD = 109;
    constexpr uint8_t ITEM_REMOVE = 110;
    constexpr uint8_t ITEM_UPDATE = 111;
    constexpr uint8_t STAT_UPDATE = 112;
    constexpr uint8_t SKILL_LEVEL = 113;
    constexpr uint8_t QUEST_INFO = 114;
}

/**
 * Paket buffer - Serialize/Deserialize için
 */
class PacketBuffer {
public:
    PacketBuffer() : m_readPos(0) {
        m_data.reserve(1024);
    }

    explicit PacketBuffer(std::vector<uint8_t> data)
        : m_data(std::move(data)), m_readPos(0) {}

    // Write operations
    void writeUint8(uint8_t value) {
        m_data.push_back(value);
    }

    void writeUint16(uint16_t value) {
        m_data.push_back(static_cast<uint8_t>(value & 0xFF));
        m_data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    }

    void writeUint32(uint32_t value) {
        m_data.push_back(static_cast<uint8_t>(value & 0xFF));
        m_data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        m_data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        m_data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }

    void writeInt32(int32_t value) {
        writeUint32(static_cast<uint32_t>(value));
    }

    void writeString(const std::string& str, size_t maxLen = 0) {
        if (maxLen > 0) {
            // Fixed length string
            size_t len = std::min(str.size(), maxLen - 1);
            m_data.insert(m_data.end(), str.begin(), str.begin() + len);
            m_data.resize(m_data.size() + (maxLen - len), 0);
        } else {
            // Variable length (with size prefix)
            writeUint16(static_cast<uint16_t>(str.size()));
            m_data.insert(m_data.end(), str.begin(), str.end());
        }
    }

    void writeBytes(const void* data, size_t size) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), bytes, bytes + size);
    }

    // Read operations
    uint8_t readUint8() {
        if (m_readPos >= m_data.size()) return 0;
        return m_data[m_readPos++];
    }

    uint16_t readUint16() {
        if (m_readPos + 2 > m_data.size()) return 0;
        uint16_t value = m_data[m_readPos] | (m_data[m_readPos + 1] << 8);
        m_readPos += 2;
        return value;
    }

    uint32_t readUint32() {
        if (m_readPos + 4 > m_data.size()) return 0;
        uint32_t value = m_data[m_readPos] |
                        (m_data[m_readPos + 1] << 8) |
                        (m_data[m_readPos + 2] << 16) |
                        (m_data[m_readPos + 3] << 24);
        m_readPos += 4;
        return value;
    }

    int32_t readInt32() {
        return static_cast<int32_t>(readUint32());
    }

    std::string readString(size_t maxLen = 0) {
        if (maxLen > 0) {
            // Fixed length
            if (m_readPos + maxLen > m_data.size()) return "";
            std::string str(reinterpret_cast<const char*>(&m_data[m_readPos]));
            m_readPos += maxLen;
            return str;
        } else {
            // Variable length
            uint16_t len = readUint16();
            if (m_readPos + len > m_data.size()) return "";
            std::string str(reinterpret_cast<const char*>(&m_data[m_readPos]), len);
            m_readPos += len;
            return str;
        }
    }

    void readBytes(void* dest, size_t size) {
        if (m_readPos + size > m_data.size()) return;
        std::memcpy(dest, &m_data[m_readPos], size);
        m_readPos += size;
    }

    // Utilities
    const uint8_t* data() const { return m_data.data(); }
    size_t size() const { return m_data.size(); }
    size_t remaining() const { return m_data.size() - m_readPos; }
    void clear() { m_data.clear(); m_readPos = 0; }
    void reset() { m_readPos = 0; }

private:
    std::vector<uint8_t> m_data;
    size_t m_readPos;
};

/**
 * Paket örnekleri
 */

// Login paketi
#pragma pack(push, 1)
struct LoginPacket {
    PacketHeader header;
    char username[25];
    char password[17];

    LoginPacket() {
        header.id = ClientPackets::LOGIN;
        header.size = sizeof(LoginPacket);
        std::memset(username, 0, sizeof(username));
        std::memset(password, 0, sizeof(password));
    }
};
#pragma pack(pop)

// Move paketi
#pragma pack(push, 1)
struct MovePacket {
    PacketHeader header;
    int32_t x;
    int32_t y;
    uint32_t timestamp;

    MovePacket(int32_t x_ = 0, int32_t y_ = 0)
        : x(x_), y(y_), timestamp(0) {
        header.id = ClientPackets::MOVE;
        header.size = sizeof(MovePacket);
    }
};
#pragma pack(pop)

// Character info paketi
#pragma pack(push, 1)
struct CharacterInfoPacket {
    PacketHeader header;
    uint32_t id;
    char name[25];
    uint8_t job;
    uint8_t level;
    uint32_t exp;
    int32_t x, y;
    uint32_t hp, maxHp;
    uint32_t sp, maxSp;
    uint16_t st, ht, dx, iq; // Statlar

    CharacterInfoPacket() {
        header.id = ServerPackets::CHARACTER_INFO;
        header.size = sizeof(CharacterInfoPacket);
        std::memset(name, 0, sizeof(name));
    }
};
#pragma pack(pop)

// Chat paketi
struct ChatPacket {
    PacketHeader header;
    uint8_t type; // 0=Normal, 1=Guild, 2=Party, 3=Whisper
    uint32_t senderId;
    std::string message;

    void serialize(PacketBuffer& buffer) const {
        buffer.writeUint8(header.id);
        buffer.writeUint16(header.size);
        buffer.writeUint8(type);
        buffer.writeUint32(senderId);
        buffer.writeString(message, 256);
    }

    bool deserialize(PacketBuffer& buffer) {
        header.id = buffer.readUint8();
        header.size = buffer.readUint16();
        type = buffer.readUint8();
        senderId = buffer.readUint32();
        message = buffer.readString(256);
        return true;
    }
};

} // namespace metin2::network

/**
 * Kullanım örneği:
 *
 * // Paket oluşturma
 * LoginPacket loginPkt;
 * strncpy(loginPkt.username, "player1", sizeof(loginPkt.username) - 1);
 * strncpy(loginPkt.password, "pass123", sizeof(loginPkt.password) - 1);
 *
 * // Gönderme
 * send(socket, &loginPkt, sizeof(loginPkt), 0);
 *
 * // PacketBuffer kullanımı
 * PacketBuffer buffer;
 * buffer.writeUint8(ClientPackets::CHAT);
 * buffer.writeString("Merhaba!");
 */
