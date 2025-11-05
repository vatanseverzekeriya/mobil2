#pragma once

#include "entity.hpp"
#include "metin2/core/types.hpp"
#include <string>
#include <array>
#include <map>

namespace metin2::entity {

/**
 * Karakter statları
 */
struct CharacterStats {
    uint32_t level = 1;
    uint64_t exp = 0;

    // Temel statlar
    uint32_t st = 1;  // Strength
    uint32_t ht = 1;  // Health
    uint32_t dx = 1;  // Dexterity
    uint32_t iq = 1;  // Intelligence

    // HP/SP
    int32_t hp = 100;
    int32_t maxHp = 100;
    int32_t sp = 100;
    int32_t maxSp = 100;

    // Combat stats
    uint32_t attackPower = 0;
    uint32_t magicPower = 0;
    uint32_t defense = 0;
    uint16_t attackSpeed = 100;
    uint16_t moveSpeed = 100;

    // Bonuslar
    int16_t criticalPct = 0;
    int16_t piercingPct = 0;
    int16_t bonusHp = 0;
    int16_t bonusSp = 0;

    // Dirençler
    int16_t fireResist = 0;
    int16_t iceResist = 0;
    int16_t windResist = 0;
    int16_t elecResist = 0;
    int16_t darkResist = 0;
};

/**
 * Skill bilgisi
 */
struct SkillData {
    uint32_t vnum;           // Skill ID
    uint8_t level;           // Skill seviyesi
    uint8_t masterType;      // 0=Normal, 1=Master, 2=Grand Master
    core::TimePoint cooldownEnd; // Cooldown bitişi
};

/**
 * Affect (Buff/Debuff)
 */
struct Affect {
    uint32_t type;
    int32_t value;
    uint32_t duration;  // Saniye
    core::TimePoint startTime;

    bool isExpired() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
        return elapsed.count() >= duration;
    }
};

/**
 * Character class - Oyuncu ve NPC'ler için temel sınıf
 */
class Character : public Entity {
public:
    Character(core::EntityId id, const std::string& name);
    virtual ~Character() = default;

    // Temel bilgiler
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    core::CharacterClass getClass() const { return m_class; }
    void setClass(core::CharacterClass cls) { m_class = cls; }

    core::Empire getEmpire() const { return m_empire; }
    void setEmpire(core::Empire empire) { m_empire = empire; }

    // Statlar
    const CharacterStats& getStats() const { return m_stats; }
    CharacterStats& getStats() { return m_stats; }
    void setStats(const CharacterStats& stats) { m_stats = stats; }

    // HP/SP
    int32_t getHp() const { return m_stats.hp; }
    int32_t getMaxHp() const { return m_stats.maxHp; }
    void setHp(int32_t hp);
    void modifyHp(int32_t delta);
    bool isDead() const { return m_stats.hp <= 0; }

    int32_t getSp() const { return m_stats.sp; }
    int32_t getMaxSp() const { return m_stats.maxSp; }
    void setSp(int32_t sp);
    void modifySp(int32_t delta);

    // Level & Exp
    uint32_t getLevel() const { return m_stats.level; }
    uint64_t getExp() const { return m_stats.exp; }
    void addExp(uint64_t amount);
    void levelUp();

    // Gold
    uint64_t getGold() const { return m_gold; }
    void setGold(uint64_t gold) { m_gold = gold; }
    bool addGold(uint64_t amount);
    bool removeGold(uint64_t amount);

    // Skills
    bool hasSkill(uint32_t vnum) const;
    const SkillData* getSkill(uint32_t vnum) const;
    void addSkill(uint32_t vnum, uint8_t level);
    void removeSkill(uint32_t vnum);
    bool canUseSkill(uint32_t vnum) const;
    void useSkill(uint32_t vnum, core::EntityId targetId);

    // Affects (Buffs/Debuffs)
    void addAffect(const Affect& affect);
    void removeAffect(uint32_t type);
    bool hasAffect(uint32_t type) const;
    const Affect* getAffect(uint32_t type) const;
    void updateAffects();

    // Combat
    virtual void attack(core::EntityId targetId);
    virtual void takeDamage(int32_t damage, core::EntityId attackerId);
    virtual void die(core::EntityId killerId);
    virtual void respawn();

    // Combat state
    bool isInCombat() const { return m_inCombat; }
    void setInCombat(bool combat) { m_inCombat = combat; }

    core::EntityId getTarget() const { return m_targetId; }
    void setTarget(core::EntityId targetId) { m_targetId = targetId; }

    // Movement
    bool isMoving() const { return m_isMoving; }
    void startMoving(const core::Position& destination);
    void stopMoving();

    // Update
    void update(float deltaTime) override;

    // Stat calculation (bonuslar dahil)
    int32_t calculateAttackPower() const;
    int32_t calculateDefense() const;
    int32_t calculateMagicPower() const;

    // Debug
    std::string toString() const override;

protected:
    std::string m_name;
    core::CharacterClass m_class;
    core::Empire m_empire;
    CharacterStats m_stats;
    uint64_t m_gold;

    // Skills
    std::map<uint32_t, SkillData> m_skills;

    // Affects
    std::map<uint32_t, Affect> m_affects;

    // Combat state
    bool m_inCombat;
    core::EntityId m_targetId;
    core::TimePoint m_lastAttackTime;

    // Movement
    bool m_isMoving;
    core::Position m_destination;
    float m_moveProgress;
};

/**
 * Player Character - Oyuncu karakteri
 */
class PlayerCharacter : public Character {
public:
    PlayerCharacter(core::EntityId id, const std::string& name, uint32_t accountId);
    ~PlayerCharacter() override = default;

    // Account
    uint32_t getAccountId() const { return m_accountId; }

    // Player-specific features
    uint32_t getPlayTime() const { return m_playTime; }
    void addPlayTime(uint32_t seconds) { m_playTime += seconds; }

    // Guild
    uint32_t getGuildId() const { return m_guildId; }
    void setGuildId(uint32_t guildId) { m_guildId = guildId; }

    // Party
    uint32_t getPartyId() const { return m_partyId; }
    void setPartyId(uint32_t partyId) { m_partyId = partyId; }

    // PK mode
    uint8_t getPkMode() const { return m_pkMode; }
    void setPkMode(uint8_t mode) { m_pkMode = mode; }

    // Shop coins (premium currency)
    uint32_t getCoins() const { return m_coins; }
    void setCoins(uint32_t coins) { m_coins = coins; }

    // Stat points (level up için)
    uint16_t getStatPoint() const { return m_statPoint; }
    void addStatPoint(uint16_t points) { m_statPoint += points; }
    bool useStat(uint8_t statType);

    // Skill points
    uint16_t getSkillPoint() const { return m_skillPoint; }
    void addSkillPoint(uint16_t points) { m_skillPoint += points; }

    // Update
    void update(float deltaTime) override;

private:
    uint32_t m_accountId;
    uint32_t m_playTime;
    uint32_t m_guildId;
    uint32_t m_partyId;
    uint8_t m_pkMode;
    uint32_t m_coins;
    uint16_t m_statPoint;
    uint16_t m_skillPoint;
};

/**
 * Monster - NPC mob
 */
class Monster : public Character {
public:
    Monster(core::EntityId id, uint32_t vnum);
    ~Monster() override = default;

    uint32_t getVnum() const { return m_vnum; }

    // AI state
    enum class AIState {
        Idle,
        Wandering,
        Chasing,
        Attacking,
        Fleeing,
        Dead
    };

    AIState getAIState() const { return m_aiState; }
    void setAIState(AIState state) { m_aiState = state; }

    // Aggro
    void addAggro(core::EntityId entityId, int32_t amount);
    void removeAggro(core::EntityId entityId);
    core::EntityId getTopAggro() const;

    // Spawn
    const core::Position& getSpawnPoint() const { return m_spawnPoint; }
    void setSpawnPoint(const core::Position& pos) { m_spawnPoint = pos; }

    // Update (AI logic)
    void update(float deltaTime) override;

private:
    uint32_t m_vnum;
    AIState m_aiState;
    core::Position m_spawnPoint;
    std::map<core::EntityId, int32_t> m_aggroTable;
    float m_aiUpdateTimer;
};

} // namespace metin2::entity

/**
 * Kullanım örneği:
 *
 * // Player oluşturma
 * auto player = std::make_shared<PlayerCharacter>(1001, "Warrior1", 123);
 * player->setClass(CharacterClass::Warrior);
 * player->setEmpire(Empire::Red);
 * player->setPosition(1000, 2000);
 *
 * // Stat ayarlama
 * auto& stats = player->getStats();
 * stats.level = 10;
 * stats.st = 20;
 * stats.hp = 500;
 * stats.maxHp = 500;
 *
 * // Skill ekleme
 * player->addSkill(1, 5); // Skill vnum 1, level 5
 *
 * // Affect ekleme
 * Affect buffAffect{100, 50, 60, std::chrono::steady_clock::now()};
 * player->addAffect(buffAffect);
 *
 * // Monster oluşturma
 * auto monster = std::make_shared<Monster>(2001, 101); // Mob vnum 101
 * monster->setName("Wolf");
 * monster->setSpawnPoint({1500, 2500});
 *
 * // Combat
 * player->attack(monster->getId());
 * monster->takeDamage(50, player->getId());
 */
