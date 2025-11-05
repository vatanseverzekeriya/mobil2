#pragma once

#include "metin2/core/types.hpp"
#include <array>
#include <string>
#include <memory>

namespace metin2::entity {

/**
 * Item tipleri
 */
enum class ItemType : uint8_t {
    None = 0,
    Weapon = 1,
    Armor = 2,
    Use = 3,      // Kullanılabilir item (potion vb.)
    Autouse = 4,  // Otomatik kullanılan
    Material = 5,
    Special = 6,
    Tool = 7,
    Lottery = 8,
    ELK = 9,      // Para (gold drop)
    Metin = 10,   // Metin taşı
    Container = 11,
    Fish = 12,
    Rod = 13,
    Resource = 14,
    Campfire = 15,
    Unique = 16,
    Skillbook = 17,
    Chest = 18,
    Ring = 19,
    Belt = 20,
    Costume = 21,
    DS = 22,      // Dragon Soul
    Special_DS = 23,
    Extract = 24,
    Gacha = 25
};

/**
 * Item alt tipleri (weapon)
 */
enum class WeaponSubType : uint8_t {
    Sword = 0,
    Dagger = 1,
    Bow = 2,
    TwoHandSword = 3,
    Bell = 4,
    Fan = 5,
    Arrow = 6,
    MountSpear = 7,
    Claw = 8
};

/**
 * Item alt tipleri (armor)
 */
enum class ArmorSubType : uint8_t {
    Body = 0,
    Head = 1,
    Shield = 2,
    Wrist = 3,
    Foots = 4,
    Neck = 5,
    Ear = 6
};

/**
 * Item flag'leri
 */
namespace ItemFlags {
    constexpr uint32_t STACKABLE     = (1 << 0);  // Yığılabilir
    constexpr uint32_t COUNT_PER_USE = (1 << 1);  // Kullanımda azalır
    constexpr uint32_t SAVE          = (1 << 2);  // Kaydedilir
    constexpr uint32_t UNIQUE        = (1 << 3);  // Tek olabilir
    constexpr uint32_t SLOW_QUERY    = (1 << 4);  // Yavaş DB sorgusu
    constexpr uint32_t RARE          = (1 << 5);  // Nadir
    constexpr uint32_t LOG           = (1 << 6);  // Loglanır
}

/**
 * Item anti-flag (kullanım kısıtlamaları)
 */
namespace ItemAntiFlags {
    constexpr uint32_t WARRIOR = (1 << 0);
    constexpr uint32_t NINJA   = (1 << 1);
    constexpr uint32_t SURA    = (1 << 2);
    constexpr uint32_t SHAMAN  = (1 << 3);
    constexpr uint32_t MALE    = (1 << 4);
    constexpr uint32_t FEMALE  = (1 << 5);
    constexpr uint32_t DROP    = (1 << 6);  // Drop edilemez
    constexpr uint32_t SELL    = (1 << 7);  // Satılamaz
    constexpr uint32_t GIVE    = (1 << 8);  // Verilemez
}

/**
 * Item attribute (bonus) tipleri
 */
enum class ItemAttribute : uint8_t {
    None = 0,
    MaxHP = 1,
    MaxSP = 2,
    STR = 3,
    DEX = 4,
    INT = 5,
    CON = 6,
    AttackSpeed = 7,
    MoveSpeed = 8,
    CriticalPct = 9,
    // ... daha fazla
};

/**
 * Item socket tipleri
 */
enum class ItemSocket : uint8_t {
    None = 0,
    NormalMetin = 1,
    GoldMetin = 2,
    Time = 3,         // Expire time
    RealTime = 4,
    Remain = 5,       // Kalan kullanım
    // ... daha fazla
};

/**
 * Item proto (blueprint)
 * Veritabanından yüklenen item tanımı
 */
struct ItemProto {
    uint32_t vnum;
    std::string name;
    std::string localeName;

    ItemType type;
    uint8_t subType;

    uint8_t size;           // Inventory'de kapladığı yer
    uint32_t flags;
    uint32_t antiFlags;
    uint32_t wearFlags;     // Hangi slota giyilebilir

    // Değerler
    uint32_t gold;          // Fiyat
    uint32_t shopBuyPrice;
    uint32_t refineVnum;    // Geliştirme itemı

    // Limitler
    uint8_t limitType[2];
    int32_t limitValue[2];

    // Apply bonusları (item giyildiğinde)
    ItemAttribute applyType[6];
    int32_t applyValue[6];

    // Socket sayısı
    uint8_t socketCount;

    // Kullanım
    uint32_t refined;       // Refined sonuç vnum
    uint16_t weight;
    uint32_t specular;
    uint32_t socket[3];     // Default socket değerleri

    // İstatistikler (silah için)
    int32_t attackPower;
    int32_t magicPower;
    int32_t defense;

    ItemProto() : vnum(0), type(ItemType::None), subType(0) {
        std::memset(limitType, 0, sizeof(limitType));
        std::memset(limitValue, 0, sizeof(limitValue));
        std::memset(applyType, 0, sizeof(applyType));
        std::memset(applyValue, 0, sizeof(applyValue));
        std::memset(socket, 0, sizeof(socket));
    }
};

/**
 * Item instance
 * Oyundaki gerçek item
 */
class Item {
public:
    Item(core::ItemId id, uint32_t vnum);
    ~Item() = default;

    // Temel bilgiler
    core::ItemId getId() const { return m_id; }
    uint32_t getVnum() const { return m_vnum; }

    const ItemProto* getProto() const { return m_proto; }
    void setProto(const ItemProto* proto) { m_proto = proto; }

    // Owner
    core::EntityId getOwner() const { return m_ownerId; }
    void setOwner(core::EntityId ownerId) { m_ownerId = ownerId; }

    // Window & Position
    core::ItemWindow getWindow() const { return m_window; }
    void setWindow(core::ItemWindow window) { m_window = window; }

    uint8_t getPosition() const { return m_position; }
    void setPosition(uint8_t pos) { m_position = pos; }

    // Count (stackable items için)
    uint16_t getCount() const { return m_count; }
    void setCount(uint16_t count) { m_count = count; }
    bool addCount(uint16_t amount);
    bool removeCount(uint16_t amount);

    // Socket
    int32_t getSocket(uint8_t index) const {
        return index < 3 ? m_socket[index] : 0;
    }
    void setSocket(uint8_t index, int32_t value) {
        if (index < 3) m_socket[index] = value;
    }

    // Attribute (bonus)
    ItemAttribute getAttributeType(uint8_t index) const {
        return index < 7 ? m_attrType[index] : ItemAttribute::None;
    }
    int16_t getAttributeValue(uint8_t index) const {
        return index < 7 ? m_attrValue[index] : 0;
    }
    void setAttribute(uint8_t index, ItemAttribute type, int16_t value) {
        if (index < 7) {
            m_attrType[index] = type;
            m_attrValue[index] = value;
        }
    }
    void clearAttributes();

    // Upgrade level (+0, +1, ... +9)
    uint8_t getRefineLevel() const { return m_refineLevel; }
    void setRefineLevel(uint8_t level) { m_refineLevel = level; }

    // Item özellikleri
    bool isStackable() const;
    bool isEquippable() const;
    bool isWeapon() const;
    bool isArmor() const;
    bool canDrop() const;
    bool canSell() const;
    bool canGive() const;

    // Expire time
    uint32_t getExpireTime() const { return m_expireTime; }
    void setExpireTime(uint32_t time) { m_expireTime = time; }
    bool isExpired() const;

    // Kullanım
    bool canUse(core::EntityId userId) const;
    void use(core::EntityId userId);

    // Debug
    std::string toString() const;

private:
    core::ItemId m_id;
    uint32_t m_vnum;
    const ItemProto* m_proto;

    core::EntityId m_ownerId;
    core::ItemWindow m_window;
    uint8_t m_position;

    uint16_t m_count;
    uint8_t m_refineLevel;

    std::array<int32_t, 3> m_socket;
    std::array<ItemAttribute, 7> m_attrType;
    std::array<int16_t, 7> m_attrValue;

    uint32_t m_expireTime;
};

/**
 * Item Manager
 * Item proto ve instance yönetimi
 */
class ItemManager {
public:
    ItemManager() = default;
    ~ItemManager() = default;

    // Proto yükleme
    bool loadProto(const std::string& filename);
    const ItemProto* getProto(uint32_t vnum) const;

    // Item oluşturma
    std::shared_ptr<Item> createItem(uint32_t vnum, uint16_t count = 1);
    void destroyItem(core::ItemId itemId);

    // Item instance
    std::shared_ptr<Item> getItem(core::ItemId itemId);
    void addItem(std::shared_ptr<Item> item);
    void removeItem(core::ItemId itemId);

    // Owner bazlı sorgulama
    std::vector<std::shared_ptr<Item>> getItemsByOwner(core::EntityId ownerId);

    // Temizlik
    void clear();

private:
    std::unordered_map<uint32_t, ItemProto> m_protos;
    std::unordered_map<core::ItemId, std::shared_ptr<Item>> m_items;
    std::mutex m_mutex;

    static std::atomic<core::ItemId> s_nextId;
};

} // namespace metin2::entity

/**
 * Kullanım örneği:
 *
 * // Item manager
 * ItemManager itemMgr;
 * itemMgr.loadProto("item_proto.txt");
 *
 * // Item oluşturma
 * auto item = itemMgr.createItem(10, 1); // vnum 10, count 1
 * if (item) {
 *     item->setOwner(playerId);
 *     item->setWindow(ItemWindow::Inventory);
 *     item->setPosition(0); // İlk slot
 *
 *     // Bonus ekleme
 *     item->setAttribute(0, ItemAttribute::STR, 12);
 *     item->setAttribute(1, ItemAttribute::MaxHP, 500);
 *
 *     // Socket
 *     item->setSocket(0, 25040); // Metin vnum
 *
 *     // Upgrade
 *     item->setRefineLevel(5); // +5 item
 * }
 *
 * // Item kullanma
 * if (item->canUse(playerId)) {
 *     item->use(playerId);
 * }
 *
 * // Player'ın tüm itemları
 * auto playerItems = itemMgr.getItemsByOwner(playerId);
 */
