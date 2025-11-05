#pragma once

#include "metin2/core/types.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace metin2::entity {

/**
 * Entity tipi
 */
enum class EntityType : uint8_t {
    None = 0,
    Player = 1,
    Monster = 2,
    NPC = 3,
    Item = 4,
    Object = 5
};

/**
 * Component base class (ECS pattern)
 */
class Component {
public:
    virtual ~Component() = default;
    virtual void update(float deltaTime) {}
};

/**
 * Entity base class
 * Oyundaki tüm objeler için temel sınıf
 */
class Entity {
public:
    explicit Entity(core::EntityId id, EntityType type);
    virtual ~Entity();

    // Temel bilgiler
    core::EntityId getId() const { return m_id; }
    EntityType getType() const { return m_type; }

    // Pozisyon
    const core::Position& getPosition() const { return m_position; }
    void setPosition(const core::Position& pos);
    void setPosition(int32_t x, int32_t y, int32_t z = 0);

    // Hareket
    virtual void moveTo(const core::Position& pos);
    virtual void teleportTo(const core::Position& pos);

    // Yön
    float getRotation() const { return m_rotation; }
    void setRotation(float rotation) { m_rotation = rotation; }

    // Görünürlük
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    // Aktif/pasif
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    // Component sistemi
    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        m_components[typeid(T).hash_code()] = std::move(component);
        return ptr;
    }

    template<typename T>
    T* getComponent() {
        auto it = m_components.find(typeid(T).hash_code());
        if (it != m_components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent() const {
        return m_components.find(typeid(T).hash_code()) != m_components.end();
    }

    template<typename T>
    void removeComponent() {
        m_components.erase(typeid(T).hash_code());
    }

    // Update
    virtual void update(float deltaTime);

    // Mesafe hesaplama
    float distanceTo(const Entity& other) const;
    float distanceTo(const core::Position& pos) const;

    // Debug
    virtual std::string toString() const;

protected:
    core::EntityId m_id;
    EntityType m_type;
    core::Position m_position;
    float m_rotation;
    bool m_visible;
    bool m_active;

    // Components
    std::unordered_map<size_t, std::unique_ptr<Component>> m_components;
};

/**
 * Entity Factory
 */
class EntityFactory {
public:
    static std::unique_ptr<Entity> createEntity(EntityType type);
    static core::EntityId getNextId();

private:
    static std::atomic<core::EntityId> s_nextId;
};

/**
 * Entity Manager
 * Tüm entity'leri yönetir
 */
class EntityManager {
public:
    EntityManager() = default;
    ~EntityManager() = default;

    // Entity ekleme/çıkarma
    void addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(core::EntityId id);
    std::shared_ptr<Entity> getEntity(core::EntityId id);

    // Tip bazlı sorgulama
    std::vector<std::shared_ptr<Entity>> getEntitiesByType(EntityType type);

    // Alan bazlı sorgulama (spatial query)
    std::vector<std::shared_ptr<Entity>> getEntitiesInRange(
        const core::Position& center,
        float radius
    );

    // Tüm entity'leri güncelle
    void update(float deltaTime);

    // Temizlik
    void clear();

    // İstatistik
    size_t getEntityCount() const;
    size_t getEntityCountByType(EntityType type) const;

private:
    std::unordered_map<core::EntityId, std::shared_ptr<Entity>> m_entities;
    std::mutex m_mutex;
};

} // namespace metin2::entity

/**
 * Kullanım örneği:
 *
 * // Entity oluşturma
 * auto entity = std::make_shared<Entity>(123, EntityType::Player);
 * entity->setPosition(1000, 2000);
 *
 * // Component ekleme
 * entity->addComponent<RenderComponent>();
 * entity->addComponent<PhysicsComponent>();
 *
 * // Manager'a ekleme
 * EntityManager manager;
 * manager.addEntity(entity);
 *
 * // Game loop
 * manager.update(deltaTime);
 *
 * // Alan sorgusu
 * auto nearbyEntities = manager.getEntitiesInRange({1000, 2000}, 500.0f);
 */
