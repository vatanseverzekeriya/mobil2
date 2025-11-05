#pragma once

#include <memory>
#include <vector>
#include <stack>
#include <mutex>
#include <cstddef>

namespace metin2::core {

/**
 * Object Pool - Sık oluşturulan/yok edilen objeler için
 * Mobile'da memory allocation overhead'i azaltır
 */
template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(size_t initialSize = 32, size_t maxSize = 1024)
        : m_maxSize(maxSize) {
        m_pool.reserve(initialSize);
        for (size_t i = 0; i < initialSize; ++i) {
            m_pool.push_back(std::make_unique<T>());
        }
    }

    ~ObjectPool() = default;

    // Pool'dan obje al
    template<typename... Args>
    std::shared_ptr<T> acquire(Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);

        T* raw = nullptr;

        if (!m_pool.empty()) {
            raw = m_pool.back().release();
            m_pool.pop_back();
        } else {
            raw = new T();
        }

        // Constructor çağır (placement new kullanılabilir)
        if constexpr (sizeof...(Args) > 0) {
            // Reset fonksiyonu varsa kullan
            if constexpr (requires { raw->reset(std::forward<Args>(args)...); }) {
                raw->reset(std::forward<Args>(args)...);
            }
        }

        // Custom deleter ile shared_ptr oluştur
        return std::shared_ptr<T>(raw, [this](T* ptr) {
            this->release(ptr);
        });
    }

    // Pool'a geri ver
    void release(T* obj) {
        if (obj == nullptr) return;

        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_pool.size() < m_maxSize) {
            // Cleanup fonksiyonu varsa çağır
            if constexpr (requires { obj->cleanup(); }) {
                obj->cleanup();
            }

            m_pool.push_back(std::unique_ptr<T>(obj));
        } else {
            delete obj; // Max size aşıldıysa sil
        }
    }

    // Pool istatistikleri
    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pool.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pool.clear();
    }

private:
    std::vector<std::unique_ptr<T>> m_pool;
    mutable std::mutex m_mutex;
    size_t m_maxSize;
};

/**
 * Memory Pool - Sabit boyutlu obje allocator
 * Cache-friendly, fragmentation yok
 */
class MemoryPool {
public:
    MemoryPool(size_t blockSize, size_t blockCount);
    ~MemoryPool();

    // Copy/Move engelle
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // Bellek ayır
    void* allocate();

    // Bellek geri ver
    void deallocate(void* ptr);

    // Pool bilgisi
    size_t getBlockSize() const { return m_blockSize; }
    size_t getTotalBlocks() const { return m_blockCount; }
    size_t getAvailableBlocks() const;

private:
    struct Block {
        Block* next;
    };

    void* m_memory;           // Ana bellek bloğu
    Block* m_freeList;        // Serbest bloklar
    size_t m_blockSize;       // Her bloğun boyutu
    size_t m_blockCount;      // Toplam blok sayısı
    std::mutex m_mutex;
};

/**
 * Stack Allocator - Frame-based allocation
 * Her frame'de kullan-at tipi allocations için
 */
class StackAllocator {
public:
    explicit StackAllocator(size_t size);
    ~StackAllocator();

    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;

    // Bellek ayır
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));

    // Marker'a geri sar
    void reset(size_t marker = 0);

    // Mevcut marker'ı al
    size_t getMarker() const { return m_offset; }

    // Kullanılabilir alan
    size_t getAvailableSize() const { return m_size - m_offset; }

private:
    void* m_memory;
    size_t m_size;
    size_t m_offset;
};

} // namespace metin2::core

/**
 * Kullanım örnekleri:
 *
 * // Object Pool
 * ObjectPool<Monster> monsterPool(100);
 * auto monster = monsterPool.acquire();
 * // monster otomatik olarak pool'a geri döner
 *
 * // Memory Pool
 * MemoryPool itemPool(sizeof(Item), 1000);
 * Item* item = static_cast<Item*>(itemPool.allocate());
 * new (item) Item(); // placement new
 * // ...
 * item->~Item();
 * itemPool.deallocate(item);
 *
 * // Stack Allocator
 * StackAllocator frameAlloc(1024 * 1024); // 1MB
 * void* buffer = frameAlloc.allocate(256);
 * // Frame sonunda
 * frameAlloc.reset();
 */
