# ✈️ AirGate System

AirGate System, modern havalimanı yönetimi ve yolcu deneyimini dijitalleştirmek için geliştirilmiş kapsamlı bir masaüstü uygulamasıdır. Qt/C++ ile geliştirilen bu proje, havalimanı operasyonlarını kolaylaştırmak ve kullanıcı dostu bir arayüz sunmak amacıyla tasarlanmıştır.

## 🚀 Özellikler

- **Çoklu Rol Desteği:** Hostes, pilot ve yolcu için özelleştirilmiş arayüzler
- **Akıllı Kart Okuyucu Entegrasyonu:** Yolcu ve personel tanımlama işlemleri
- **Veritabanı Yönetimi:** Yolcu, uçuş, rol ve diğer bilgiler için güvenli veri saklama
- **Döviz ve Hava Durumu Bilgileri:** Anlık güncellenen bilgilerle yolcu bilgilendirme
- **Rol Atama:** Kullanıcıların yetkilerine göre dinamik rol atama
- **Modern ve Şık Arayüz:** Qt Designer ile hazırlanmış kullanıcı dostu ekranlar

## 🖥️ Ekran Görüntüleri

> Ekran görüntüleri ve demo GIF'leri ekleyerek projenizi daha çekici hale getirebilirsiniz.

## 📁 Proje Yapısı

- `main.cpp` : Uygulamanın giriş noktası
- `mainwindow.*` : Ana pencere ve genel arayüz yönetimi
- `assignrole_dialog.*` : Rol atama diyalogu
- `rolepage_*.*` : Farklı kullanıcı rolleri için arayüzler (hostes, pilot, yolcu)
- `cardreader.*` : Kart okuyucu entegrasyonu
- `database.*` : Veritabanı işlemleri
- `currencyinfo.*` : Döviz bilgileri
- `weatherinfo.*` : Hava durumu bilgileri
- `newsinfo.*` : Haberler ve bilgilendirme
- `style.qss` : Uygulama stil dosyası

## ⚙️ Kurulum

1. **Projeyi Klonlayın:**
   ```bash
   git clone https://github.com/ilhanuzunoglu/airgate_system.git
   cd airgate_system
   ```
2. **Qt ile Açın:**
   - Qt Creator veya terminal üzerinden açabilirsiniz.
3. **Derleyin ve Çalıştırın:**
   - Gerekli Qt kütüphanelerinin kurulu olduğundan emin olun.

## 🛠️ Kullanılan Teknolojiler

- Qt 5/6 (C++)
- Modern C++
- SQLite veya diğer desteklenen veritabanları

## 👨‍💻 Katkıda Bulunma

Katkılarınızı bekliyoruz! Lütfen önce bir issue açın ve değişikliklerinizi tartışalım.

## 📄 Lisans

MIT Lisansı ile lisanslanmıştır.

---

> **İletişim:** ilhanuzunoglu02@gmail.com

Havalimanı yönetiminde yeni bir çağ başlatın! 🚀 