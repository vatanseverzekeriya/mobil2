# Metin2 Modüler ve Mobile Uyumlu Mimari

## Genel Bakış

Orijinal Metin2 monolitik yapıdan modern, modüler ve cross-platform bir mimariye geçiş.

## Proje Yapısı

```
metin2-mobile/
├── CMakeLists.txt                 # Ana build dosyası
├── README.md
├── docs/                          # Dokümantasyon
│   ├── architecture.md
│   ├── mobile-adaptation.md
│   └── api-reference.md
│
├── external/                      # 3. parti kütüphaneler
│   ├── CMakeLists.txt
│   ├── spdlog/                   # Loglama
│   ├── asio/                     # Network (Boost.Asio standalone)
│   ├── sqlite3/                  # Mobile DB
│   ├── protobuf/                 # Protokol
│   └── nlohmann-json/            # JSON
│
├── core/                          # Çekirdek modül
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/core/
│   │       ├── types.hpp
│   │       ├── singleton.hpp
│   │       ├── event_system.hpp
│   │       ├── memory_pool.hpp
│   │       └── logger.hpp
│   └── src/
│       ├── event_system.cpp
│       ├── memory_pool.cpp
│       └── logger.cpp
│
├── platform/                      # Platform soyutlama
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/platform/
│   │       ├── platform.hpp
│   │       ├── file_system.hpp
│   │       ├── thread.hpp
│   │       └── network_info.hpp
│   └── src/
│       ├── common/               # Ortak implementasyon
│       ├── android/              # Android özel
│       ├── ios/                  # iOS özel
│       ├── windows/              # Windows özel
│       └── linux/                # Linux özel
│
├── network/                       # Network modülü
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/network/
│   │       ├── session.hpp
│   │       ├── packet.hpp
│   │       ├── protocol.hpp
│   │       └── server_connection.hpp
│   └── src/
│       ├── session.cpp
│       ├── packet.cpp
│       └── server_connection.cpp
│
├── entity/                        # Entity sistemi (ECS yaklaşımı)
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/entity/
│   │       ├── entity.hpp
│   │       ├── component.hpp
│   │       ├── character.hpp
│   │       ├── item.hpp
│   │       ├── monster.hpp
│   │       └── inventory.hpp
│   └── src/
│       ├── entity.cpp
│       ├── character.cpp
│       ├── item.cpp
│       ├── monster.cpp
│       └── inventory.cpp
│
├── combat/                        # Savaş sistemi
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/combat/
│   │       ├── damage_calculator.hpp
│   │       ├── skill.hpp
│   │       └── affect.hpp
│   └── src/
│       ├── damage_calculator.cpp
│       ├── skill.cpp
│       └── affect.cpp
│
├── world/                         # Dünya/Harita sistemi
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/world/
│   │       ├── map.hpp
│   │       ├── sector.hpp
│   │       └── spawn_manager.hpp
│   └── src/
│       ├── map.cpp
│       ├── sector.cpp
│       └── spawn_manager.cpp
│
├── quest/                         # Quest sistemi
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/quest/
│   │       ├── quest_manager.hpp
│   │       ├── quest.hpp
│   │       └── lua_binding.hpp
│   └── src/
│       ├── quest_manager.cpp
│       ├── quest.cpp
│       └── lua_binding.cpp
│
├── database/                      # Veritabanı katmanı
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/database/
│   │       ├── database.hpp
│   │       ├── repository.hpp
│   │       ├── player_repository.hpp
│   │       └── item_repository.hpp
│   └── src/
│       ├── sqlite_database.cpp
│       ├── player_repository.cpp
│       └── item_repository.cpp
│
├── ui/                            # UI abstraction (mobil için)
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── metin2/ui/
│   │       ├── ui_manager.hpp
│   │       ├── screen.hpp
│   │       └── touch_handler.hpp
│   └── src/
│       ├── ui_manager.cpp
│       └── touch_handler.cpp
│
├── game-client/                   # Client uygulaması
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── game_client.hpp
│   └── src/
│       └── main.cpp
│
├── game-server/                   # Server uygulaması (test için)
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── game_server.hpp
│   └── src/
│       └── main.cpp
│
└── tests/                         # Unit testler
    ├── CMakeLists.txt
    ├── core/
    ├── network/
    ├── entity/
    └── combat/
```

## Modüllerin Bağımlılık Grafiği

```
game-client/game-server
    ↓
ui/quest/combat/world
    ↓
entity
    ↓
database/network
    ↓
platform
    ↓
core
```

## İsimlendirme Kuralları

### Dosya İsimlendirme
- Header dosyaları: `.hpp` (C++ standardı)
- Kaynak dosyaları: `.cpp`
- Küçük harf + alt çizgi: `player_repository.hpp`

### Sınıf İsimlendirme
- PascalCase: `PlayerCharacter`, `ItemRepository`
- Interface'ler: `IDatabase`, `INetworkSession`

### Namespace İsimlendirme
- Küçük harf: `metin2::core`, `metin2::network`
- Her modül kendi namespace'ine sahip

### Fonksiyon/Değişken
- camelCase: `getPlayerName()`, `maxHealth`
- Private üyeler: `m_` prefix: `m_health`

## Platform Makroları

```cpp
// Platform detection
#if defined(__ANDROID__)
    #define M2_PLATFORM_ANDROID
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define M2_PLATFORM_IOS
    #else
        #define M2_PLATFORM_MACOS
    #endif
#elif defined(_WIN32)
    #define M2_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define M2_PLATFORM_LINUX
#endif

// Mobile check
#if defined(M2_PLATFORM_ANDROID) || defined(M2_PLATFORM_IOS)
    #define M2_MOBILE
#endif
```

## Modern C++ Özellikleri

- **C++17 minimum**, C++20 tercih edilir
- Smart pointers: `std::unique_ptr`, `std::shared_ptr`
- Move semantics
- RAII pattern
- Template metaprogramming
- Concepts (C++20)
- Modules (C++20 - opsiyonel)

## Mobile Optimizasyonlar

### Bellek Yönetimi
- Object pooling
- Memory-mapped files
- Küçük allocations için custom allocator

### Performans
- Multi-threading (std::thread, std::async)
- SIMD optimizasyonları (ARM NEON, SSE)
- Cache-friendly data structures

### Enerji Tasarrufu
- Update rate dinamik ayarlama
- Background'da network activity azaltma
- GPU kullanımı optimizasyonu
