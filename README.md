# Metin2 Mobile - Modern C++ Modular Architecture

Metin2 MMORPG'nin modern, modüler ve cross-platform (özellikle mobile-uyumlu) C++20 implementasyonu.

## 🎯 Proje Hedefleri

- ✅ Modern C++ (C++20) standartları
- ✅ Modüler mimari (loosely coupled)
- ✅ Cross-platform (Windows, Linux, Android, iOS)
- ✅ Mobile-optimized (bellek, performans, batarya)
- ✅ Kolay bakım ve genişletilebilirlik

## 📁 Proje Yapısı

```
metin2-mobile/
├── core/                   # Çekirdek modül (types, logger, events, memory)
├── platform/              # Platform soyutlama (Android, iOS, Windows, Linux)
├── network/               # Network katmanı (session, packet, protocol)
├── entity/                # Entity sistemi (character, item, monster)
├── combat/                # Savaş mekanikleri
├── world/                 # Harita ve dünya sistemi
├── quest/                 # Quest sistemi (Lua entegrasyonu)
├── database/              # Veritabanı katmanı
├── ui/                    # UI abstraction
├── game-client/           # Client uygulaması
├── game-server/           # Server uygulaması
├── tests/                 # Unit testler
└── external/              # 3. parti kütüphaneler
```

## 🏗️ Modüller

### Core Module
**Bağımlılık:** Yok (base module)

Temel tipler, singleton pattern, event sistemi, memory pooling, logging.

```cpp
#include <metin2/core/types.hpp>
#include <metin2/core/logger.hpp>
#include <metin2/core/event_system.hpp>

// Kullanım
M2_LOG_INFO("Server started on port", 13000);

EventManager eventMgr;
eventMgr.subscribe<CharacterDiedEvent>([](const Event& e) {
    // Handle death
});
```

### Platform Module
**Bağımlılık:** Core

Platform-specific işlemler için abstraction layer (dosya sistemi, battery, network info).

```cpp
#include <metin2/platform/platform.hpp>

auto& platform = getPlatform();
auto info = platform.getPlatformInfo();

#ifdef M2_MOBILE
    float battery = platform.getBatteryLevel();
    platform.vibrate(100);
#endif
```

### Network Module
**Bağımlılık:** Core

TCP/IP session yönetimi, paket serialize/deserialize, protocol handling.

```cpp
#include <metin2/network/session.hpp>
#include <metin2/network/packet.hpp>

auto session = std::make_shared<Session>(socket);
session->setPacketHandler(ServerPackets::LOGIN_SUCCESS, [](auto data, auto size) {
    M2_LOG_INFO("Login successful!");
});

LoginPacket pkt;
session->sendPacket(pkt);
```

### Entity Module
**Bağımlılık:** Core, Network

Entity Component System (ECS), karakter, item, monster, inventory yönetimi.

```cpp
#include <metin2/entity/character.hpp>
#include <metin2/entity/item.hpp>

auto player = std::make_shared<PlayerCharacter>(1001, "Warrior1", 123);
player->setClass(CharacterClass::Warrior);
player->getStats().level = 10;

auto item = itemMgr.createItem(10, 1); // vnum 10
item->setOwner(player->getId());
```

## 🚀 Derleme

### Gereksinimler

- CMake 3.18+
- C++20 destekleyen compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Platform-specific SDK'lar:
  - Android: Android NDK r21+
  - iOS: Xcode 12+

### Desktop (Linux/Windows)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Android

```bash
export ANDROID_NDK=/path/to/ndk

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DBUILD_CLIENT=ON

make -j8
```

### iOS

```bash
cmake .. \
    -GXcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DBUILD_CLIENT=ON

xcodebuild -project Metin2Mobile.xcodeproj \
    -scheme metin2_client \
    -configuration Release
```

## 📚 Dokümantasyon

- [MODULAR_ARCHITECTURE.md](MODULAR_ARCHITECTURE.md) - Modüler yapı detayları
- [MOBILE_ADAPTATION_GUIDE.md](MOBILE_ADAPTATION_GUIDE.md) - Mobile optimizasyon rehberi
- [METIN2_FILE_STRUCTURE.md](METIN2_FILE_STRUCTURE.md) - Orijinal Metin2 dosya yapısı

## 🔧 CMake Seçenekleri

```bash
# Modül seçenekleri
-DBUILD_COMBAT_MODULE=ON      # Savaş modülü
-DBUILD_WORLD_MODULE=ON        # Dünya modülü
-DBUILD_QUEST_MODULE=ON        # Quest modülü
-DBUILD_DATABASE_MODULE=ON     # Veritabanı modülü
-DBUILD_UI_MODULE=ON           # UI modülü

# Uygulama seçenekleri
-DBUILD_CLIENT=ON              # Client uygulaması
-DBUILD_SERVER=OFF             # Server uygulaması
-DBUILD_TESTS=OFF              # Unit testler
```

## 🎨 Kod Örnekleri

### Event-Driven Architecture

```cpp
// Event tanımlama
struct PlayerLoginEvent : public Event {
    uint32_t playerId;
    std::string playerName;

    std::string getName() const override { return "PlayerLoginEvent"; }
};

// Event subscriber
eventMgr.subscribe<PlayerLoginEvent>([](const Event& e) {
    const auto& login = static_cast<const PlayerLoginEvent&>(e);
    M2_LOG_INFO("Player", login.playerName, "logged in");
});

// Event publish
eventMgr.publish(PlayerLoginEvent{1001, "Warrior1"});
```

### Memory Pool Usage

```cpp
// Packet pool
ObjectPool<Packet> packetPool(100);

// Acquire from pool
auto packet = packetPool.acquire();
packet->setType(PacketType::Move);
// ... packet kullanımı

// Otomatik olarak pool'a döner (shared_ptr custom deleter)
```

### Entity Component System

```cpp
// Component tanımlama
class RenderComponent : public Component {
public:
    void update(float deltaTime) override {
        // Render logic
    }
};

// Entity'e ekleme
auto entity = std::make_shared<Entity>(123, EntityType::Player);
entity->addComponent<RenderComponent>();

// Component erişimi
if (auto* render = entity->getComponent<RenderComponent>()) {
    render->update(deltaTime);
}
```

### Multi-Threading

```cpp
TaskScheduler scheduler;

// Paralel AI güncellemesi
for (auto& monster : monsters) {
    scheduler.addTask([&monster] {
        monster->updateAI();
    });
}

scheduler.waitAll();
```

## 📱 Mobile Optimizasyonlar

### Bellek Optimizasyonu
- Object pooling (50-70% allocation azalması)
- Texture streaming (RAM kullanımı -60%)
- Entity culling (görünmeyen objeler işlenmez)
- Memory budget system (kategori bazlı limit)

### Performans
- Dinamik FPS (30-60 FPS arası, batarya durumuna göre)
- Multi-threading (AI, physics, network paralel)
- Cache-friendly data structures (SoA pattern)
- SIMD optimizasyonları (ARM NEON)

### Enerji Tasarrufu
- Update rate throttling (arka planda 1 FPS)
- GPU throttling (değişiklik yoksa render yok)
- Network batching (paket gruplayıp gönderme)
- Low power mode detection

### UI/UX
- Touch gesture support (tap, swipe, pinch)
- Adaptive layout (phone/tablet)
- Minimum 48dp touch targets
- Safe area support (notch, navigation bar)

## 🧪 Testing

```bash
# Unit testler
cmake .. -DBUILD_TESTS=ON
make -j8
ctest --output-on-failure

# Bellek sızıntısı kontrolü (Valgrind)
valgrind --leak-check=full ./metin2_client

# Profiling (perf)
perf record ./metin2_client
perf report
```

## 📊 Performans Hedefleri

### Desktop
- 60 FPS @ 1920x1080
- 500+ karakterler ekranda
- <1GB RAM kullanımı
- <50ms network latency

### Mobile
- 30-60 FPS (adaptif)
- 50-100 karakter ekranda
- <512MB RAM kullanımı
- Batarya: 3-4 saat gameplay

## 🛠️ Geliştirme Araçları

### IDE Setup
- CLion: CMake entegrasyonu built-in
- Visual Studio: CMake Tools extension
- VSCode: C++ extension + CMake Tools
- Xcode: CMake generator ile proje oluştur

### Linting & Formatting
```bash
# clang-format
clang-format -i **/*.cpp **/*.hpp

# clang-tidy
clang-tidy src/*.cpp -- -std=c++20
```

### Static Analysis
```bash
# cppcheck
cppcheck --enable=all --std=c++20 src/

# clang static analyzer
scan-build cmake ..
scan-build make
```

## 🤝 Katkıda Bulunma

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

## 📝 Coding Standards

- C++20 modern features kullan
- Smart pointers (unique_ptr, shared_ptr)
- RAII pattern
- const correctness
- Move semantics
- Header guards yerine `#pragma once`
- Naming: PascalCase (class), camelCase (function/variable)

## 🔗 Bağımlılıklar

- **CMake** - Build system
- **spdlog** - Fast logging (optional)
- **nlohmann-json** - JSON parsing
- **sqlite3** - Mobile database
- **protobuf** - Protocol buffers (optional)
- **asio** - Async networking (optional)

## 📄 Lisans

Bu proje eğitim amaçlıdır. Orijinal Metin2 içeriği Webzen'e aittir.

## 🙏 Teşekkürler

- Metin2 community
- Modern C++ open-source projects
- Mobile game optimization resources

## 📞 İletişim

- Issues: GitHub Issues
- Discussions: GitHub Discussions

---

**Not:** Bu proje Metin2'nin modern, modüler bir reimplementasyonudur. Production kullanımı için ek güvenlik, anti-cheat ve stabilite çalışmaları gereklidir.
