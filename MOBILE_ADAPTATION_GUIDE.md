# Metin2 Mobile Adaptasyon Rehberi

## İçindekiler
1. [Genel Yaklaşım](#genel-yaklaşım)
2. [Bellek Optimizasyonu](#bellek-optimizasyonu)
3. [Performans Optimizasyonu](#performans-optimizasyonu)
4. [Enerji Tasarrufu](#enerji-tasarrufu)
5. [UI/UX Adaptasyonu](#uiux-adaptasyonu)
6. [Network Optimizasyonu](#network-optimizasyonu)
7. [Platform-Specific İyileştirmeler](#platform-specific-iyileştirmeler)

---

## Genel Yaklaşım

### Mobil vs Desktop Farkları

| Özellik | Desktop | Mobile |
|---------|---------|--------|
| RAM | 8-32 GB | 2-8 GB |
| CPU | 4-16 core | 2-8 core (ARM) |
| GPU | Dedicated | Integrated |
| Batarya | Sınırsız | Sınırlı (2000-5000 mAh) |
| Ekran | Büyük (1920x1080+) | Küçük (720p-1440p) |
| Input | Mouse + Keyboard | Touch |
| Network | Stabil | WiFi/4G/5G (değişken) |

### Temel Prensipler

1. **Memory-First Design**: RAM kullanımını minimize et
2. **Battery-Aware**: CPU/GPU kullanımını optimize et
3. **Network-Efficient**: Paket boyutlarını küçült
4. **Touch-Optimized**: Büyük dokunma alanları
5. **Progressive Loading**: İçeriği kademeli yükle

---

## Bellek Optimizasyonu

### 1. Object Pooling

Sık oluşturulan objeler için pool kullan:

```cpp
// Örnek: Damage text pool
class DamageTextPool {
public:
    DamageTextPool() {
        // Başlangıçta 50 obje oluştur
        for (int i = 0; i < 50; ++i) {
            m_pool.push(std::make_unique<DamageText>());
        }
    }

    std::shared_ptr<DamageText> acquire(int damage, Position pos) {
        std::unique_ptr<DamageText> obj;

        if (!m_pool.empty()) {
            obj = std::move(m_pool.front());
            m_pool.pop();
        } else {
            obj = std::make_unique<DamageText>();
        }

        obj->reset(damage, pos);

        return std::shared_ptr<DamageText>(obj.release(), [this](DamageText* p) {
            if (m_pool.size() < MAX_POOL_SIZE) {
                m_pool.push(std::unique_ptr<DamageText>(p));
            } else {
                delete p;
            }
        });
    }

private:
    std::queue<std::unique_ptr<DamageText>> m_pool;
    static constexpr size_t MAX_POOL_SIZE = 100;
};
```

### 2. Texture Streaming

Texture'ları ihtiyaç anında yükle:

```cpp
class TextureManager {
public:
    void update(const Position& cameraPos) {
        // Kameraya yakın texture'ları yükle
        for (auto& [id, texture] : m_textures) {
            float distance = texture.position.distanceTo(cameraPos);

            if (distance < LOAD_DISTANCE && !texture.loaded) {
                loadTexture(texture);
            } else if (distance > UNLOAD_DISTANCE && texture.loaded) {
                unloadTexture(texture);
            }
        }
    }

    // Mipmap kullanımı
    void loadTexture(Texture& tex, float distance) {
        int mipmapLevel = calculateMipmapLevel(distance);
        tex.load(mipmapLevel); // Uzaktaysa düşük çözünürlük
    }

private:
    static constexpr float LOAD_DISTANCE = 500.0f;
    static constexpr float UNLOAD_DISTANCE = 1000.0f;
};
```

### 3. Entity Culling

Görünmeyen entity'leri işleme:

```cpp
class EntityCullingSystem {
public:
    void update(const Camera& camera) {
        m_visibleEntities.clear();

        for (auto& entity : m_allEntities) {
            // Frustum culling
            if (camera.isInFrustum(entity->getPosition())) {
                // Distance culling
                float dist = camera.distanceTo(entity->getPosition());

                if (dist < MAX_VISIBLE_DISTANCE) {
                    m_visibleEntities.push_back(entity);

                    // LOD (Level of Detail)
                    if (dist < 200.0f) {
                        entity->setLOD(LOD::High);
                    } else if (dist < 500.0f) {
                        entity->setLOD(LOD::Medium);
                    } else {
                        entity->setLOD(LOD::Low);
                    }
                }
            }
        }
    }

private:
    std::vector<std::shared_ptr<Entity>> m_allEntities;
    std::vector<std::shared_ptr<Entity>> m_visibleEntities;

    #ifdef M2_MOBILE
        static constexpr float MAX_VISIBLE_DISTANCE = 800.0f;
    #else
        static constexpr float MAX_VISIBLE_DISTANCE = 1500.0f;
    #endif
};
```

### 4. Memory Budget System

Bellek kullanımını takip et:

```cpp
class MemoryBudget {
public:
    enum class Category {
        Textures,
        Models,
        Audio,
        Scripts,
        Other
    };

    bool allocate(Category category, size_t size) {
        size_t& used = m_usedMemory[category];
        size_t limit = m_budgetLimits[category];

        if (used + size > limit) {
            // Yer yok, cleanup yap
            cleanup(category, size);
        }

        if (used + size <= limit) {
            used += size;
            return true;
        }

        return false;
    }

    void deallocate(Category category, size_t size) {
        m_usedMemory[category] -= size;
    }

private:
    void cleanup(Category category, size_t needed) {
        // LRU (Least Recently Used) temizliği
        // En az kullanılan asset'leri sil
    }

    std::map<Category, size_t> m_budgetLimits = {
        #ifdef M2_MOBILE
            {Category::Textures, 512 * 1024 * 1024},  // 512 MB
            {Category::Models, 256 * 1024 * 1024},    // 256 MB
            {Category::Audio, 64 * 1024 * 1024},      // 64 MB
        #else
            {Category::Textures, 2048 * 1024 * 1024}, // 2 GB
            {Category::Models, 1024 * 1024 * 1024},   // 1 GB
            {Category::Audio, 256 * 1024 * 1024},     // 256 MB
        #endif
    };

    std::map<Category, size_t> m_usedMemory;
};
```

---

## Performans Optimizasyonu

### 1. Frame Rate Kontrol

Mobilde dinamik FPS:

```cpp
class FrameRateManager {
public:
    void update() {
        auto& platform = getPlatform();

        // Battery level'a göre FPS ayarla
        float batteryLevel = platform.getBatteryLevel();
        bool lowPowerMode = platform.isLowPowerMode();

        if (lowPowerMode || batteryLevel < 0.2f) {
            // Düşük batarya - 30 FPS
            setTargetFPS(30);
        } else if (m_isInCombat) {
            // Savaşta - 60 FPS
            setTargetFPS(60);
        } else if (m_isMoving) {
            // Hareket - 45 FPS
            setTargetFPS(45);
        } else {
            // Idle - 30 FPS
            setTargetFPS(30);
        }
    }

    void setTargetFPS(uint32_t fps) {
        m_targetFrameTime = std::chrono::milliseconds(1000 / fps);
    }

    bool shouldUpdate() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - m_lastFrame;

        if (elapsed >= m_targetFrameTime) {
            m_lastFrame = now;
            return true;
        }

        return false;
    }

private:
    std::chrono::milliseconds m_targetFrameTime{33}; // 30 FPS default
    std::chrono::steady_clock::time_point m_lastFrame;
    bool m_isInCombat = false;
    bool m_isMoving = false;
};
```

### 2. Multi-Threading

İşleri thread'lere dağıt:

```cpp
class TaskScheduler {
public:
    TaskScheduler() {
        uint32_t threadCount = std::thread::hardware_concurrency();

        #ifdef M2_MOBILE
            // Mobilde bir çekirdek UI için ayrıl
            threadCount = std::max(1u, threadCount - 1);
        #endif

        for (uint32_t i = 0; i < threadCount; ++i) {
            m_workers.emplace_back([this] { workerThread(); });
        }
    }

    template<typename F>
    void addTask(F&& task) {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_tasks.emplace(std::forward<F>(task));
        }
        m_condition.notify_one();
    }

    void waitAll() {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_finishCondition.wait(lock, [this] {
            return m_tasks.empty() && m_activeTasks == 0;
        });
    }

private:
    void workerThread() {
        while (m_running) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_condition.wait(lock, [this] {
                    return !m_running || !m_tasks.empty();
                });

                if (!m_running && m_tasks.empty()) {
                    return;
                }

                task = std::move(m_tasks.front());
                m_tasks.pop();
                ++m_activeTasks;
            }

            task();

            {
                std::lock_guard<std::mutex> lock(m_queueMutex);
                --m_activeTasks;
            }
            m_finishCondition.notify_all();
        }
    }

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::condition_variable m_finishCondition;
    std::atomic<bool> m_running{true};
    std::atomic<uint32_t> m_activeTasks{0};
};

// Kullanım
TaskScheduler scheduler;

// AI hesaplamalarını paralel yap
for (auto& monster : monsters) {
    scheduler.addTask([&monster] {
        monster->updateAI();
    });
}

scheduler.waitAll();
```

### 3. Cache-Friendly Data Structures

SoA (Structure of Arrays) yaklaşımı:

```cpp
// Kötü (AoS - Array of Structures)
struct CharacterBad {
    uint32_t id;
    std::string name;      // Cache miss!
    Position pos;
    int32_t hp;
    int32_t maxHp;
    // ... daha fazla
};
std::vector<CharacterBad> characters;

// İyi (SoA - Structure of Arrays)
struct CharacterArrays {
    std::vector<uint32_t> ids;
    std::vector<Position> positions;
    std::vector<int32_t> hps;
    std::vector<int32_t> maxHps;
    // String'ler ayrı (nadiren erişilen)
    std::unordered_map<uint32_t, std::string> names;
};

// Update loop - cache friendly!
void updatePositions(CharacterArrays& chars, float deltaTime) {
    for (size_t i = 0; i < chars.positions.size(); ++i) {
        // Tüm position'lar ardışık bellekte
        chars.positions[i].x += deltaTime * 100;
    }
}
```

### 4. SIMD Optimizations (ARM NEON)

```cpp
#ifdef M2_ARCH_ARM64
#include <arm_neon.h>

// 4 karakterin HP'sini aynı anda güncelle
void updateHealthBatch(int32_t* hps, int32_t* damages, size_t count) {
    size_t i = 0;

    // 4'lü gruplar halinde işle
    for (; i + 4 <= count; i += 4) {
        int32x4_t hp_vec = vld1q_s32(&hps[i]);
        int32x4_t dmg_vec = vld1q_s32(&damages[i]);

        // HP - damage
        int32x4_t result = vsubq_s32(hp_vec, dmg_vec);

        // 0'dan küçük olmasın
        int32x4_t zero = vdupq_n_s32(0);
        result = vmaxq_s32(result, zero);

        vst1q_s32(&hps[i], result);
    }

    // Kalan elemanlar
    for (; i < count; ++i) {
        hps[i] = std::max(0, hps[i] - damages[i]);
    }
}
#endif
```

---

## Enerji Tasarrufu

### 1. Update Rate Throttling

Arka plandayken yavaşlat:

```cpp
class ApplicationLifecycle {
public:
    enum class State {
        Active,       // Ön planda, full speed
        Background,   // Arka planda, minimum
        Paused        // Tamamen durdu
    };

    void setState(State state) {
        m_state = state;

        switch (state) {
            case State::Active:
                m_updateInterval = std::chrono::milliseconds(33); // 30 FPS
                resumeNetwork();
                resumeAudio();
                break;

            case State::Background:
                m_updateInterval = std::chrono::milliseconds(1000); // 1 FPS
                pauseAudio();
                reduceNetworkActivity();
                break;

            case State::Paused:
                m_updateInterval = std::chrono::milliseconds(10000); // 0.1 FPS
                pauseNetwork();
                pauseAudio();
                saveState();
                break;
        }
    }

    bool shouldUpdate() {
        auto now = std::chrono::steady_clock::now();
        if (now - m_lastUpdate >= m_updateInterval) {
            m_lastUpdate = now;
            return true;
        }
        return false;
    }

private:
    State m_state = State::Active;
    std::chrono::milliseconds m_updateInterval{33};
    std::chrono::steady_clock::time_point m_lastUpdate;
};
```

### 2. GPU Throttling

Gereksiz render işlemlerini engelle:

```cpp
class RenderThrottler {
public:
    bool shouldRender() {
        // Ekran visible değilse render yapma
        if (!m_screenVisible) {
            return false;
        }

        // Değişiklik yoksa render yapma
        if (!m_sceneDirty && m_framesWithoutChange > 3) {
            return false;
        }

        // Low power mode'da FPS düşür
        auto& platform = getPlatform();
        if (platform.isLowPowerMode()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = now - m_lastRender;

            if (elapsed < std::chrono::milliseconds(100)) {
                return false; // Max 10 FPS low power'da
            }
        }

        m_lastRender = std::chrono::steady_clock::now();
        m_framesWithoutChange = m_sceneDirty ? 0 : m_framesWithoutChange + 1;
        m_sceneDirty = false;

        return true;
    }

    void markDirty() {
        m_sceneDirty = true;
        m_framesWithoutChange = 0;
    }

private:
    bool m_screenVisible = true;
    bool m_sceneDirty = true;
    uint32_t m_framesWithoutChange = 0;
    std::chrono::steady_clock::time_point m_lastRender;
};
```

---

## UI/UX Adaptasyonu

### 1. Touch Input System

```cpp
class TouchInput {
public:
    struct Touch {
        uint32_t id;
        Position position;
        Position startPosition;
        float pressure;
        std::chrono::steady_clock::time_point startTime;

        float getSwipeDistance() const {
            return position.distanceTo(startPosition);
        }

        std::chrono::milliseconds getDuration() const {
            auto now = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                now - startTime);
        }
    };

    void onTouchDown(uint32_t id, Position pos) {
        m_activeTouches[id] = Touch{id, pos, pos, 1.0f,
                                    std::chrono::steady_clock::now()};
    }

    void onTouchMove(uint32_t id, Position pos) {
        if (auto it = m_activeTouches.find(id); it != m_activeTouches.end()) {
            it->second.position = pos;
        }
    }

    void onTouchUp(uint32_t id) {
        if (auto it = m_activeTouches.find(id); it != m_activeTouches.end()) {
            Touch& touch = it->second;

            // Gesture detection
            float swipeDistance = touch.getSwipeDistance();
            auto duration = touch.getDuration();

            if (swipeDistance > 100.0f && duration.count() < 500) {
                // Swipe gesture
                onSwipe(touch);
            } else if (duration.count() < 300 && swipeDistance < 50.0f) {
                // Tap gesture
                onTap(touch);
            } else if (duration.count() > 500 && swipeDistance < 50.0f) {
                // Long press
                onLongPress(touch);
            }

            m_activeTouches.erase(it);
        }
    }

    // Pinch zoom için
    float getPinchDistance() {
        if (m_activeTouches.size() == 2) {
            auto it1 = m_activeTouches.begin();
            auto it2 = std::next(it1);
            return it1->second.position.distanceTo(it2->second.position);
        }
        return 0.0f;
    }

private:
    std::unordered_map<uint32_t, Touch> m_activeTouches;

    void onTap(const Touch& touch);
    void onSwipe(const Touch& touch);
    void onLongPress(const Touch& touch);
};
```

### 2. Adaptive UI Layout

```cpp
class UILayout {
public:
    void calculateLayout() {
        auto& platform = getPlatform();

        uint32_t screenWidth, screenHeight;
        platform.getScreenSize(screenWidth, screenHeight);

        float dpi = platform.getScreenDPI();
        float scale = dpi / 160.0f; // Android baseline

        // Touch hedefleri minimum 48dp (9mm fiziksel)
        m_minTouchTarget = 48.0f * scale;

        // Telefon vs tablet
        float screenInches = std::sqrt(
            screenWidth * screenWidth + screenHeight * screenHeight) / dpi;

        if (screenInches < 7.0f) {
            // Phone layout
            m_buttonSize = 64.0f * scale;
            m_fontSize = 14.0f * scale;
            m_padding = 8.0f * scale;
        } else {
            // Tablet layout
            m_buttonSize = 80.0f * scale;
            m_fontSize = 18.0f * scale;
            m_padding = 16.0f * scale;
        }

        // Safe area (notch, navigation bar)
        calculateSafeArea();
    }

private:
    float m_minTouchTarget;
    float m_buttonSize;
    float m_fontSize;
    float m_padding;
};
```

---

## Network Optimizasyonu

### 1. Paket Compression

```cpp
class PacketCompressor {
public:
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) {
        // Küçük paketleri sıkıştırma
        if (data.size() < 100) {
            return data;
        }

        // zlib, lz4, veya özel compression
        return compressWithLZ4(data);
    }

    std::vector<uint8_t> decompress(const std::vector<uint8_t>& data) {
        return decompressWithLZ4(data);
    }

private:
    std::vector<uint8_t> compressWithLZ4(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressWithLZ4(const std::vector<uint8_t>& data);
};
```

### 2. Adaptive Network Quality

```cpp
class NetworkQualityManager {
public:
    void update() {
        // Ping ve packet loss ölç
        uint32_t ping = measurePing();
        float packetLoss = measurePacketLoss();

        // Kalite seviyesi belirle
        if (ping < 50 && packetLoss < 0.01f) {
            m_quality = Quality::Excellent;
            m_updateRate = 20; // 20 update/sec
        } else if (ping < 100 && packetLoss < 0.05f) {
            m_quality = Quality::Good;
            m_updateRate = 15;
        } else if (ping < 200 && packetLoss < 0.10f) {
            m_quality = Quality::Fair;
            m_updateRate = 10;
        } else {
            m_quality = Quality::Poor;
            m_updateRate = 5;
        }

        // Detay seviyesini ayarla
        adjustDetailLevel();
    }

    void adjustDetailLevel() {
        switch (m_quality) {
            case Quality::Excellent:
                // Full detail
                m_sendCharacterAnimation = true;
                m_sendParticleEffects = true;
                break;

            case Quality::Good:
                // Normal detail
                m_sendCharacterAnimation = true;
                m_sendParticleEffects = false;
                break;

            case Quality::Fair:
                // Low detail
                m_sendCharacterAnimation = false;
                m_sendParticleEffects = false;
                break;

            case Quality::Poor:
                // Minimum detail
                m_sendCharacterAnimation = false;
                m_sendParticleEffects = false;
                // Sadece kritik update'ler
                break;
        }
    }

private:
    enum class Quality {
        Excellent,
        Good,
        Fair,
        Poor
    };

    Quality m_quality = Quality::Good;
    uint32_t m_updateRate = 15;
    bool m_sendCharacterAnimation = true;
    bool m_sendParticleEffects = true;
};
```

---

## Platform-Specific İyileştirmeler

### Android

```java
// Java tarafı (MainActivity.java)
public class Metin2Activity extends Activity {
    static {
        System.loadLibrary("metin2");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // C++ platform'u initialize et
        nativeInitPlatform();

        // Battery optimization
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        if (pm.isPowerSaveMode()) {
            nativeSetLowPowerMode(true);
        }

        // Network monitoring
        registerNetworkCallback();
    }

    private void registerNetworkCallback() {
        ConnectivityManager cm = (ConnectivityManager)
            getSystemService(Context.CONNECTIVITY_SERVICE);

        NetworkRequest request = new NetworkRequest.Builder()
            .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
            .build();

        cm.registerNetworkCallback(request, new ConnectivityManager.NetworkCallback() {
            @Override
            public void onAvailable(Network network) {
                nativeOnNetworkAvailable();
            }

            @Override
            public void onLost(Network network) {
                nativeOnNetworkLost();
            }
        });
    }

    // Native methods
    private native void nativeInitPlatform();
    private native void nativeSetLowPowerMode(boolean enable);
    private native void nativeOnNetworkAvailable();
    private native void nativeOnNetworkLost();
}
```

### iOS

```objc
// iOS tarafı (AppDelegate.m)
@implementation AppDelegate

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    // C++ platform initialize
    metin2_ios_init_platform();

    // Battery monitoring
    [[UIDevice currentDevice] setBatteryMonitoringEnabled:YES];

    [[NSNotificationCenter defaultCenter]
        addObserver:self
        selector:@selector(batteryLevelChanged:)
        name:UIDeviceBatteryLevelDidChangeNotification
        object:nil];

    // Low power mode
    [[NSNotificationCenter defaultCenter]
        addObserver:self
        selector:@selector(lowPowerModeChanged:)
        name:NSProcessInfoPowerStateDidChangeNotification
        object:nil];

    return YES;
}

- (void)batteryLevelChanged:(NSNotification *)notification {
    float level = [[UIDevice currentDevice] batteryLevel];
    metin2_ios_battery_changed(level);
}

- (void)lowPowerModeChanged:(NSNotification *)notification {
    BOOL enabled = [[NSProcessInfo processInfo] isLowPowerModeEnabled];
    metin2_ios_low_power_mode(enabled);
}

@end
```

---

## Build Komutları

### Android (CMake + Gradle)

```bash
# Android NDK kurulu olmalı
export ANDROID_NDK=/path/to/ndk

# Build
mkdir build-android
cd build-android

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DBUILD_CLIENT=ON

make -j8

# APK oluştur
cd ../android
./gradlew assembleDebug
```

### iOS (Xcode)

```bash
# iOS build
mkdir build-ios
cd build-ios

cmake .. \
    -GXcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DBUILD_CLIENT=ON

# Xcode'da aç veya xcodebuild ile derle
xcodebuild -project Metin2Mobile.xcodeproj \
    -scheme metin2_client \
    -configuration Release \
    -sdk iphoneos
```

---

## Performans Profiling

### Android

```bash
# Simpleperf ile profiling
adb shell simpleperf record -p <pid> -o /data/local/tmp/perf.data

# Data'yı çek
adb pull /data/local/tmp/perf.data .

# Analiz
simpleperf report -i perf.data
```

### iOS

```bash
# Instruments ile profiling
instruments -t "Time Profiler" -D output.trace YourApp.app

# Memory leaks
instruments -t "Leaks" -D leaks.trace YourApp.app
```

---

## Sonuç

Mobile adaptasyon için temel prensipler:

1. **Bellek** - Pool kullan, texture streaming, entity culling
2. **Performans** - Multi-threading, cache-friendly data, SIMD
3. **Enerji** - Dinamik FPS, throttling, background optimizasyonu
4. **UI** - Touch-optimized, adaptive layout, gesture support
5. **Network** - Compression, adaptive quality, efficient protocols

Her optimizasyon için **profiling** yap ve **ölç**. Premature optimization yapmak yerine, gerçek bottleneck'leri bul ve onları optimize et.
