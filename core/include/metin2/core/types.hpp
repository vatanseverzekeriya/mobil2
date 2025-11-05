#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace metin2::core {

// Temel tipler
using EntityId = uint32_t;
using PlayerId = uint32_t;
using ItemId = uint32_t;
using MapId = uint16_t;
using VNum = uint32_t; // Virtual Number (item/mob proto ID)

// Invalid değerler
constexpr EntityId INVALID_ENTITY_ID = 0;
constexpr PlayerId INVALID_PLAYER_ID = 0;
constexpr ItemId INVALID_ITEM_ID = 0;

// Zaman tipi
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::milliseconds;

// Pozisyon
struct Position {
    int32_t x;
    int32_t y;
    int32_t z;

    Position() : x(0), y(0), z(0) {}
    Position(int32_t x_, int32_t y_, int32_t z_ = 0)
        : x(x_), y(y_), z(z_) {}

    // Mesafe hesaplama
    float distanceTo(const Position& other) const;

    // Operatörler
    Position operator+(const Position& other) const;
    Position operator-(const Position& other) const;
    bool operator==(const Position& other) const;
};

// Oyun sabitleri
namespace constants {
    constexpr uint32_t MAX_LEVEL = 120;
    constexpr uint32_t MAX_INVENTORY_SIZE = 180;
    constexpr uint32_t MAX_SKILL_COUNT = 64;
    constexpr uint32_t MAX_PARTY_SIZE = 8;
    constexpr uint32_t MAX_GUILD_MEMBER = 32;

    // Mobile için optimize edilmiş değerler
    #ifdef M2_MOBILE
        constexpr uint32_t MAX_VISIBLE_ENTITIES = 50;
        constexpr uint32_t UPDATE_FREQUENCY_MS = 100; // 10 FPS
    #else
        constexpr uint32_t MAX_VISIBLE_ENTITIES = 200;
        constexpr uint32_t UPDATE_FREQUENCY_MS = 33; // 30 FPS
    #endif
}

// Karakter sınıfları (job)
enum class CharacterClass : uint8_t {
    Warrior = 0,
    Ninja = 1,
    Sura = 2,
    Shaman = 3
};

// İmparatorluk
enum class Empire : uint8_t {
    None = 0,
    Red = 1,
    Yellow = 2,
    Blue = 3
};

// Item pencere tipleri
enum class ItemWindow : uint8_t {
    Inventory = 0,
    Equipment = 1,
    Belt = 2,
    Ground = 3
};

// Sonuç tipleri (error handling)
template<typename T>
struct Result {
    T value;
    bool success;
    std::string error;

    static Result<T> Ok(T val) {
        return {std::move(val), true, ""};
    }

    static Result<T> Error(std::string err) {
        return {T{}, false, std::move(err)};
    }

    bool isOk() const { return success; }
    bool isError() const { return !success; }
};

// Void Result (operasyon sonucu)
struct VoidResult {
    bool success;
    std::string error;

    static VoidResult Ok() {
        return {true, ""};
    }

    static VoidResult Error(std::string err) {
        return {false, std::move(err)};
    }

    bool isOk() const { return success; }
    bool isError() const { return !success; }
};

} // namespace metin2::core
