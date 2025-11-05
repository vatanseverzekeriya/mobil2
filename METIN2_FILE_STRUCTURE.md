# Metin2 Sızdırılmış Server ve Client Dosya Yapısı

## İçindekiler
1. [Server Dosya Yapısı](#server-dosya-yapısı)
2. [Client Dosya Yapısı](#client-dosya-yapısı)
3. [Dosya İlişkileri](#dosya-ilişkileri)
4. [Modüler Yapı](#modüler-yapı)

---

## Server Dosya Yapısı

### 1. **src/ (Kaynak Kod Klasörü)**
Server'ın C++ kaynak kodlarının bulunduğu ana klasör.

#### 1.1 **game/** - Oyun Sunucusu
Oyunun ana mantık katmanı. Oyuncu etkileşimleri, savaş sistemi, ticaret vb.

**Önemli Dosyalar:**
- `main.cpp` - Game server'ın giriş noktası
- `char.cpp/h` - Karakter yönetimi (hareket, saldırı, beceriler)
- `char_battle.cpp` - Savaş mekanikleri
- `char_skill.cpp` - Beceri sistemi
- `item.cpp/h` - Item yönetimi
- `item_manager.cpp` - Item oluşturma ve dağıtım
- `packet.h` - İstemci-sunucu iletişim paketleri
- `config.cpp` - Sunucu konfigürasyonu
- `db.cpp/h` - Veritabanı bağlantı katmanı
- `guild.cpp` - Lonca sistemi
- `party.cpp` - Parti sistemi
- `shop.cpp` - NPC dükkan sistemi
- `exchange.cpp` - Oyuncular arası ticaret
- `battle.cpp` - Savaş hesaplamaları
- `affect.cpp` - Buff/debuff sistemi
- `quest_manager.cpp` - Quest yönetimi
- `mob_manager.cpp` - Mob spawn ve AI
- `sectree.cpp` - Harita bölgeleme (optimizasyon)
- `p2p.cpp` - Sunucular arası iletişim
- `empire.cpp` - İmparatorluk sistemi

**İşlevi:**
- Oyuncu hareketlerini işler
- Savaş mekaniklerini hesaplar
- Item/envanter yönetimi
- Quest sistemi
- Mob AI ve spawn
- Oyuncu etkileşimleri (ticaret, parti, lonca)

#### 1.2 **db/** - Veritabanı Sunucusu
Veritabanı işlemlerini yöneten middleware.

**Önemli Dosyalar:**
- `Main.cpp` - DB server giriş noktası
- `ClientManager.cpp/h` - Game server'lardan gelen istekleri yönetir
- `DBManager.cpp/h` - MySQL bağlantı havuzu
- `Cache.cpp` - Veritabanı önbelleği
- `QID.h` - Query ID tanımları
- `Peer.cpp` - Game server bağlantıları

**İşlevi:**
- MySQL veritabanına erişim sağlar
- Karakter verilerini cache'ler
- Oyuncu kaydetme/yükleme
- Item veritabanı işlemleri
- Log kayıtları

#### 1.3 **common/** - Ortak Kütüphaneler
Hem game hem db sunucusu tarafından kullanılan ortak kod.

**Önemli Dosyalar:**
- `tables.h` - Veri yapı tanımları
- `length.h` - Sabit değerler (max karakter adı uzunluğu vb.)
- `item_length.h` - Item sabitleri
- `CommonDefines.h` - Ortak tanımlamalar
- `service.h` - Sunucu servisleri

**İşlevi:**
- Ortak veri yapıları
- Sabitler ve enumerasyonlar
- Platform bağımsız kod

#### 1.4 **libthecore/** - Temel Kütüphane
Düşük seviye network ve sistem fonksiyonları.

**Önemli Dosyalar:**
- `socket.cpp` - Socket yönetimi
- `buffer.cpp` - Veri buffer yönetimi
- `fdwatch.cpp` - File descriptor izleme (epoll/kqueue)
- `utils.cpp` - Yardımcı fonksiyonlar
- `log.cpp` - Loglama sistemi

**İşlevi:**
- TCP/IP socket yönetimi
- Asenkron I/O
- Event loop
- Temel sistem fonksiyonları

#### 1.5 **libsql/** - SQL Kütüphanesi
MySQL erişim katmanı.

**Önemli Dosyalar:**
- `AsyncSQL.cpp` - Asenkron SQL sorguları
- `SQLMsg.cpp` - SQL mesaj kuyrukları

**İşlevi:**
- Asenkron veritabanı sorguları
- Connection pooling
- Query queue yönetimi

#### 1.6 **libpoly/** - Çokgen Kütüphanesi
Harita collision detection.

**İşlevi:**
- Harita bölgelerinin çokgen tanımları
- Oyuncunun hareket edebileceği alanları belirler

---

### 2. **share/ (Ortak Klasör)**
Server'ın çalışması için gerekli veri dosyaları.

#### 2.1 **data/** - Oyun Verileri
```
data/
├── locale/
│   ├── turkey/           # Türkçe yerelleştirme
│   │   ├── quest/       # Quest script'leri
│   │   ├── mob_names    # Mob isimleri
│   │   └── item_names   # Item isimleri
│   └── [diğer diller]/
├── monster/             # Mob proto dosyaları
│   └── mob_proto
├── item/               # Item proto dosyaları
│   └── item_proto
├── group/              # Mob grup tanımları
│   └── group.txt
└── dungeon/           # Dungeon ayarları
```

**İşlevi:**
- Oyun içi verilerin tanımlanması
- Çoklu dil desteği
- Item/Mob özellikleri

#### 2.2 **quest/** - Quest Script'leri
Lua dilinde yazılmış quest dosyaları.

**Örnek Yapı:**
```
quest/
├── main_quest_lv1.quest
├── main_quest_lv2.quest
├── subquest_*.quest
└── npc/
    ├── merchant.quest
    └── warehouse.quest
```

**İşlevi:**
- NPC diyalogları
- Quest mantığı
- Event sistemi
- Ödül dağıtımı

---

### 3. **CONFIG Dosyaları**

#### 3.1 **CONFIG** - Ana Konfigürasyon
```
HOSTNAME: game1
CHANNEL: 1
PORT: 13000
P2P_PORT: 14000
DB_PORT: 15000
MAP_ALLOW: 1 21 41
```

**Parametreler:**
- `HOSTNAME`: Sunucu adı
- `CHANNEL`: Kanal numarası
- `PORT`: İstemci bağlantı portu
- `P2P_PORT`: Sunucular arası iletişim
- `DB_PORT`: Veritabanı sunucu portu
- `MAP_ALLOW`: Bu sunucuda çalışacak haritalar

#### 3.2 **channel_setup.lua**
```lua
channel_setup = {
    {
        name = "CH1",
        map_index = 1,
        ip = "192.168.1.100"
    }
}
```

---

### 4. **Veritabanı Yapısı**

#### 4.1 **account** - Hesap Tablosu
```sql
- id (Primary Key)
- login (Kullanıcı adı)
- password (Şifreli)
- social_id (Güvenlik)
- email
- status (Ban durumu)
- coins (Shop puanı)
```

#### 4.2 **player** - Karakter Tablosu
```sql
- id (Primary Key)
- account_id (Foreign Key)
- name (Karakter adı)
- level
- exp
- gold
- job (Sınıf)
- st, ht, dx, iq (Statlar)
- x, y (Konum)
- hp, sp
- skill_level (Beceri seviyeleri)
```

#### 4.3 **item** - Item Tablosu
```sql
- id (Primary Key)
- owner_id (Karakter ID)
- window (EQUIPMENT, INVENTORY)
- pos (Slot pozisyonu)
- vnum (Item tipi)
- count (Miktar)
- socket0-2 (Socket değerleri)
- attrtype0-6 (Bonus tipleri)
- attrvalue0-6 (Bonus değerleri)
```

#### 4.4 **guild** - Lonca Tablosu
```sql
- id
- name
- master (Lonca başkanı)
- level
- exp
- skill_point
```

---

## Client Dosya Yapısı

### 1. **root/** - Python Script Klasörü
Client UI ve mantık Python ile yazılmıştır.

#### 1.1 **uiCommon.py**
Ortak UI bileşenleri.

**İşlevi:**
- Pencere oluşturma
- Buton, metin kutusu vb. widgetler
- Ortak UI fonksiyonları

#### 1.2 **networkModule.py**
Sunucu iletişimi.

**İşlevi:**
- Paket gönderme/alma
- Bağlantı yönetimi
- Protokol handling

#### 1.3 **game.py**
Ana oyun ekranı.

**İşlevi:**
- Karakter hareketi
- Kamera kontrolü
- Oyun input handling

#### 1.4 **ui*.py** Dosyaları
```
- uiInventory.py     # Envanter penceresi
- uiCharacter.py     # Karakter istatistikleri
- uiShop.py          # Dükkan arayüzü
- uiExchange.py      # Ticaret penceresi
- uiGuild.py         # Lonca arayüzü
- uiQuest.py         # Quest penceresi
- uiMessenger.py     # Arkadaş listesi
- uiMiniMap.py       # Mini harita
```

#### 1.5 **constInfo.py**
Sabit değerler.

**İşlevi:**
- Item tipi sabitleri
- Karakter sabitleri
- UI sabitleri

#### 1.6 **localeInfo.py**
Dil dosyaları.

**İşlevi:**
- Çoklu dil desteği
- Metin çevirileri

---

### 2. **pack/** - Kaynak Dosyaları

#### 2.1 **EterPack (EPK) Formatı**
Sıkıştırılmış arşiv dosyaları.

**Paketler:**
```
pack/
├── root.epk          # Python script'leri
├── locale_tr.epk     # Türkçe çeviriler
├── pc.epk            # Karakter modelleri
├── pc2.epk           # Ek karakter dosyaları
├── npc.epk           # NPC modelleri
├── monster.epk       # Mob modelleri
├── property.epk      # Item icon'ları
├── effect.epk        # Efekt dosyaları
├── textures.epk      # Texture'lar
├── sound.epk         # Ses efektleri
└── music.epk         # Müzikler
```

#### 2.2 **İçerik Yapısı**
```
pack/
└── [Unpack edilmiş]
    ├── d:/ymir work/   # Model dosyaları
    │   ├── pc/        # Karakter modelleri (.gr2, .msa)
    │   ├── npc/       # NPC modelleri
    │   ├── monster/   # Mob modelleri
    │   ├── item/      # Item modelleri
    │   │   ├── weapon/
    │   │   ├── armor/
    │   │   └── etc/
    │   ├── effect/    # Parçacık efektleri
    │   └── tree/      # Ağaç modelleri
    │
    ├── icon/          # UI icon'ları
    │   ├── item/     # Item icon'ları (.tga)
    │   └── skill/    # Beceri icon'ları
    │
    ├── sound/         # Ses dosyaları
    └── bgm/          # Arka plan müziği
```

---

### 3. **Miles/** - Ses Kütüphanesi
RAD Game Tools Miles Sound System.

**Dosyalar:**
- `mss32.dll` - Ana kütüphane

**İşlevi:**
- 3D ses
- Müzik çalma
- Ses efektleri

---

### 4. **UserInterface/** - C++ UI Kaynak Kodu

#### 4.1 **PythonNetworkStream.cpp**
Python-C++ network köprüsü.

**İşlevi:**
- Python'dan gelen network çağrılarını işler
- Paket encode/decode
- Bağlantı yönetimi

#### 4.2 **PythonItem.cpp**
Item sistemi C++ tarafı.

**İşlevi:**
- Item render
- Item özellik hesaplamaları

#### 4.3 **GameType.h**
Oyun sabitleri.

**İşlevi:**
- Paket tipleri enum
- Karakter sabitleri
- Item sabitleri

---

### 5. **Binary/** - Derlenmiş Dosyalar

```
bin/
├── metin2client.exe    # Ana executable
├── devil.dll           # Image loading
├── granny2.dll         # 3D model loading
├── mss32.dll           # Ses sistemi
├── python27.dll        # Python runtime
├── SpeedTreeRT.dll     # Ağaç render
└── [diğer dll'ler]
```

---

## Dosya İlişkileri

### 1. **Client → Server İletişimi**

```
[Client Python UI]
       ↓
[networkModule.py]
       ↓
[PythonNetworkStream.cpp]
       ↓
[TCP Socket]
       ↓
[Server game/input_main.cpp]
       ↓
[Paket İşleyicileri]
       ↓
[Oyun Mantığı (char.cpp, item.cpp vb.)]
```

**Örnek Paket Akışı:**
1. Oyuncu envanterinde iteme tıklar
2. `uiInventory.py` → `SendItemUsePacket()`
3. `networkModule.py` → Paketi encode eder
4. C++ network katmanı → Sunucuya gönderir
5. Server `input_main.cpp` → `CG_ITEM_USE` paketini alır
6. `input_item.cpp` → Item kullanım mantığı
7. `char_item.cpp` → Item efekti uygulanır
8. Server → Client'e güncelleme paketi (affect, stat değişimi)
9. Client Python → UI güncellenir

---

### 2. **Server İç İletişim**

```
[Game Server 1] ←→ [P2P] ←→ [Game Server 2]
       ↓                           ↓
       └────→ [DB Server] ←────────┘
                   ↓
              [MySQL DB]
```

**Örnekler:**

**Karakter Transferi:**
1. Oyuncu harita değiştirir
2. Game1 → DB Server: Karakteri kaydet
3. DB Server → MySQL: Veri yaz
4. DB Server → Game2: Karakteri yükle
5. Game2 → Client: Yeni haritaya spawn

**Lonca Chat:**
1. Oyuncu mesaj yazar (Game1)
2. Game1 → DB Server: Guild chat mesajı
3. DB Server → Tüm Game Server'lar: Mesaj yayını
4. Her Game Server → İlgili lonca üyelerine gönder

---

### 3. **Quest Sistemi İlişkisi**

```
[Quest Lua Script]
       ↓
[quest_manager.cpp] → Lua VM
       ↓
[C++ Game Functions]
       ↓
[Oyun Mantığı]
```

**Quest Örneği:**
```lua
-- kill_monster_quest.quest
quest begin_hunt begin
    state start begin
        when 20001.chat."Görev" begin
            say("10 Köpek öldür!")
            set_state(hunting)
        end
    end

    state hunting begin
        when kill with npc.get_race() == 101 begin
            local count = get_flag("kill_count") + 1
            set_flag("kill_count", count)

            if count >= 10 then
                set_state(reward)
            end
        end
    end

    state reward begin
        when 20001.chat."Ödül Al" begin
            item.add(30001, 1)  -- Kılıç ver
            clear_quest()
        end
    end
end
```

**İşlem Akışı:**
1. `quest_manager.cpp` → Lua dosyasını yükler
2. Oyuncu NPC'ye tıklar → `when 20001.chat` trigger
3. Lua → `say()` fonksiyonu → C++ `quest_manager`
4. C++ → Client'e dialog paketi
5. Oyuncu mob öldürür → `when kill` trigger
6. Lua → `set_flag()` → C++'da counter artırılır
7. Koşul sağlanınca → `item.add()` → `char_item.cpp`

---

## Modüler Yapı

### 1. **Network Katmanı**

**Sorumluluk:** İstemci-sunucu iletişimi

**Modüller:**
- `libthecore/` → Socket, buffer, fdwatch
- `game/input_*.cpp` → Paket işleyicileri
- `CLIENT_PROTOCOL` enum → Paket tipleri

**Bağımlılıklar:**
- Oyun mantığı modüllerine paket verir
- Hiçbir modüle bağımlı değil (alt katman)

---

### 2. **Veritabanı Katmanı**

**Sorumluluk:** Veri kalıcılığı ve cache

**Modüller:**
- `db/` sunucusu
- `libsql/` → MySQL wrapper
- `game/db.cpp` → DB query fonksiyonları

**Bağımlılıklar:**
- MySQL veritabanı
- Tüm oyun modülleri veri için buna bağımlı

---

### 3. **Karakter Modülü**

**Sorumluluk:** Karakter yönetimi

**Modüller:**
- `char.cpp/h` → Ana karakter sınıfı
- `char_*.cpp` → Özelleşmiş fonksiyonlar
  - `char_battle.cpp` → Savaş
  - `char_item.cpp` → Item işlemleri
  - `char_skill.cpp` → Beceriler
  - `char_affect.cpp` → Buff/debuff

**Bağımlılıklar:**
- Item modülü
- Sectree modülü (konum)
- DB modülü

---

### 4. **Item Modülü**

**Sorumluluk:** Item yönetimi

**Modüller:**
- `item.cpp/h` → Item sınıfı
- `item_manager.cpp` → Item oluşturma
- `shop.cpp` → NPC dükkanları
- `exchange.cpp` → Oyuncu ticareti

**Bağımlılıklar:**
- Karakter modülü (owner)
- DB modülü (kaydetme)

---

### 5. **Savaş Modülü**

**Sorumluluk:** Combat mekanikleri

**Modüller:**
- `battle.cpp` → Hasar hesaplamaları
- `char_battle.cpp` → Savaş mantığı
- `affect.cpp` → Durum efektleri

**Bağımlılıklar:**
- Karakter modülü
- Item modülü (silah bonusları)

---

### 6. **Quest Modülü**

**Sorumluluk:** Quest sistemi

**Modüller:**
- `quest_manager.cpp` → Ana yönetici
- `quest_lua.cpp` → Lua fonksiyonları
- `*.quest` dosyaları → Quest script'leri

**Bağımlılıklar:**
- Lua VM
- Karakter modülü
- Item modülü
- NPC modülü

---

### 7. **Harita Modülü**

**Sorumluluk:** Harita ve konum yönetimi

**Modüller:**
- `sectree.cpp` → Harita bölgeleme
- `sectree_manager.cpp` → Harita yöneticisi
- `regen.cpp` → Mob spawn

**Bağımlılıklar:**
- Karakter modülü
- Mob modülü

---

### 8. **Sosyal Modüller**

**Sorumluluk:** Çok oyunculu özellikler

**Modüller:**
- `guild.cpp` → Lonca sistemi
- `party.cpp` → Parti sistemi
- `messenger.cpp` → Arkadaş listesi
- `p2p.cpp` → Sunucular arası iletişim

**Bağımlılıklar:**
- Karakter modülü
- DB modülü
- Network modülü (sunucular arası)

---

### 9. **Client UI Modülü**

**Sorumluluk:** Kullanıcı arayüzü

**Modüller:**
- `root/ui*.py` → Python UI dosyaları
- `UserInterface/*.cpp` → C++ render backend

**Bağımlılıklar:**
- Network modülü
- Python runtime
- C++ grafik motoru

---

### 10. **Grafik Modülü**

**Sorumluluk:** 3D render

**Modüller:**
- `EterLib/` → Temel grafik fonksiyonları
- `GameLib/` → Oyun render
- `granny2.dll` → Model loading
- `SpeedTreeRT.dll` → Ağaç render

**Bağımlılıklar:**
- DirectX 9
- Model dosyaları (.gr2)
- Texture dosyaları (.dds)

---

## Geliştirme İş Akışı

### Yeni Bir Özellik Eklemek

**Örnek: Yeni bir item tipi eklemek**

1. **Veritabanı:**
   - `common/item_length.h` → Yeni item sabitini tanımla
   - `item_proto` dosyasına ekle

2. **Server:**
   - `item.cpp` → Yeni item tipine özel mantık ekle
   - `item_manager.cpp` → Yaratma fonksiyonu
   - `char_item.cpp` → Kullanım mantığı

3. **Client:**
   - `pack/icon/item/` → Icon dosyası ekle
   - `uiInventory.py` → Özel tooltip/görünüm
   - `localeInfo.py` → İsim ve açıklama

4. **Test:**
   - Server restart
   - Client pack yeniden oluştur
   - Oyun içi test

---

## Güvenlik Notları

**Kritik Dosyalar:**
- `account` tablosu → Şifreler encrypt edilmeli
- `CONFIG` → IP ve portlar
- `db/` → Sadece localhost erişimi
- Client binary → Anti-cheat koruması

**Yaygın Güvenlik Açıkları:**
- SQL injection → Prepared statements kullan
- Packet flooding → Rate limiting
- Dupe bug → Transaction kullan
- Speed hack → Server-side doğrulama

---

## Performans Optimizasyonu

### Server Tarafı
- `sectree` → Spatial partitioning
- `regen` interval ayarı
- P2P mesaj optimizasyonu
- DB query cache

### Client Tarafı
- Model LOD (Level of Detail)
- Texture streaming
- Frustum culling
- Particle limit

---

## Sonuç

Metin2 server/client yapısı karmaşık ama modüler bir mimariye sahiptir:

**Server:**
- C++ ile yazılmış, performans odaklı
- Game server (oyun mantığı) + DB server (veri yönetimi)
- Modüler yapı sayesinde kolay geliştirme

**Client:**
- Python (UI) + C++ (render/network)
- Pack sistemi ile kaynak yönetimi
- DirectX 9 tabanlı grafik

**İletişim:**
- TCP socket üzerinden binary protokol
- P2P ile multi-server desteği
- Lua ile esnek quest sistemi

Bu yapı sayesinde özel sunucular kolayca geliştirilebilir ve özelleştirilebilir.
