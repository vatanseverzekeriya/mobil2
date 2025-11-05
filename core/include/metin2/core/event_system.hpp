#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <any>

namespace metin2::core {

/**
 * Event-driven architecture için event sistemi
 * Modüller arası loose coupling sağlar
 */

// Event ID tipi
using EventId = size_t;

// Event base class
struct Event {
    virtual ~Event() = default;
    virtual std::string getName() const = 0;
};

// Event handler callback
using EventHandler = std::function<void(const Event&)>;

/**
 * Event Manager - Observer pattern implementasyonu
 */
class EventManager {
public:
    EventManager() = default;
    ~EventManager() = default;

    // Event'e abone ol
    template<typename EventType>
    EventId subscribe(EventHandler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);

        EventId id = m_nextId++;
        const char* eventName = typeid(EventType).name();

        m_handlers[eventName].emplace_back(id, std::move(handler));
        return id;
    }

    // Abonelikten çık
    void unsubscribe(EventId id);

    // Event yayınla (synchronous)
    template<typename EventType>
    void publish(const EventType& event) {
        std::vector<EventHandler> handlersCopy;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            const char* eventName = typeid(EventType).name();

            auto it = m_handlers.find(eventName);
            if (it != m_handlers.end()) {
                handlersCopy.reserve(it->second.size());
                for (const auto& [id, handler] : it->second) {
                    handlersCopy.push_back(handler);
                }
            }
        }

        // Lock dışında callback'leri çağır (deadlock önleme)
        for (const auto& handler : handlersCopy) {
            handler(event);
        }
    }

    // Event kuyruğa ekle (asynchronous, daha sonra işle)
    template<typename EventType>
    void queueEvent(std::unique_ptr<EventType> event) {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_eventQueue.push_back(std::move(event));
    }

    // Kuyruktaki eventleri işle
    void processQueue();

    // Tüm handler'ları temizle
    void clear();

private:
    using HandlerPair = std::pair<EventId, EventHandler>;

    std::unordered_map<std::string, std::vector<HandlerPair>> m_handlers;
    std::vector<std::unique_ptr<Event>> m_eventQueue;

    std::mutex m_mutex;
    std::mutex m_queueMutex;
    EventId m_nextId = 1;
};

/**
 * Önceden tanımlı event'ler
 */

// Karakter events
struct CharacterSpawnEvent : public Event {
    uint32_t characterId;
    int32_t x, y;

    CharacterSpawnEvent(uint32_t id, int32_t x_, int32_t y_)
        : characterId(id), x(x_), y(y_) {}

    std::string getName() const override { return "CharacterSpawnEvent"; }
};

struct CharacterDiedEvent : public Event {
    uint32_t characterId;
    uint32_t killerId;

    CharacterDiedEvent(uint32_t victim, uint32_t killer)
        : characterId(victim), killerId(killer) {}

    std::string getName() const override { return "CharacterDiedEvent"; }
};

struct CharacterLevelUpEvent : public Event {
    uint32_t characterId;
    uint32_t newLevel;

    CharacterLevelUpEvent(uint32_t id, uint32_t level)
        : characterId(id), newLevel(level) {}

    std::string getName() const override { return "CharacterLevelUpEvent"; }
};

// Item events
struct ItemPickedUpEvent : public Event {
    uint32_t characterId;
    uint32_t itemId;
    uint32_t vnum;

    ItemPickedUpEvent(uint32_t charId, uint32_t item, uint32_t vnum_)
        : characterId(charId), itemId(item), vnum(vnum_) {}

    std::string getName() const override { return "ItemPickedUpEvent"; }
};

struct ItemUsedEvent : public Event {
    uint32_t characterId;
    uint32_t itemId;

    ItemUsedEvent(uint32_t charId, uint32_t item)
        : characterId(charId), itemId(item) {}

    std::string getName() const override { return "ItemUsedEvent"; }
};

// Quest events
struct QuestStartedEvent : public Event {
    uint32_t characterId;
    std::string questName;

    QuestStartedEvent(uint32_t id, std::string quest)
        : characterId(id), questName(std::move(quest)) {}

    std::string getName() const override { return "QuestStartedEvent"; }
};

struct QuestCompletedEvent : public Event {
    uint32_t characterId;
    std::string questName;

    QuestCompletedEvent(uint32_t id, std::string quest)
        : characterId(id), questName(std::move(quest)) {}

    std::string getName() const override { return "QuestCompletedEvent"; }
};

} // namespace metin2::core

/**
 * Kullanım örneği:
 *
 * EventManager eventMgr;
 *
 * // Subscribe
 * auto subId = eventMgr.subscribe<CharacterDiedEvent>([](const Event& e) {
 *     const auto& event = static_cast<const CharacterDiedEvent&>(e);
 *     std::cout << "Character " << event.characterId << " died!\n";
 * });
 *
 * // Publish
 * eventMgr.publish(CharacterDiedEvent{123, 456});
 *
 * // Unsubscribe
 * eventMgr.unsubscribe(subId);
 */
