# Arduino-Nano-Wireless-NRF-RC-Car
# DIY Wireless RC Car & Custom Transmitter

Bu proje, tamamen sıfırdan tasarlanmış ve üretilmiş, kablosuz (RF) iletişimle kontrol edilen özel bir uzaktan kumandalı araba ve kumanda sistemidir. Hem donanımsal hem de yazılımsal olarak tüm süreçler (3D tasarım, devre tasarımı ve Arduino kodlaması) bu repoda yer almaktadır.

 Kullanılan Bileşenler

### Araç (Alıcı Tarafı):
* **Mikrokontrolcü:** Arduino Nano
* **Kablosuz Haberleşme:** NRF24L01 Transceiver Modülü
* **Motor Kontrolü:** Çift kanallı röle modülü & Güçlü DC Motor
* **Güç Yönetimi:** 12V Batarya ve ekranlı/voltaj göstergeli voltaj regülatörü
* **Aydınlatma:** Alt taban RGB LED aydınlatma
* **Şasi:** Özel 3D tasarım ve baskı parça bileşenleri

### Kumanda (Verici Tarafı):
* **Mikrokontrolcü:** Arduino Nano
* **Kablosuz Haberleşme:** NRF24L01 Transceiver Modülü
* **Giriş Elemanları:** Joystick ve potansiyometreler (Adaptif hız kontrolü için)

##  Özellikler
* **Özel Tasarım Kumanda:** Ergonomik, kendi üretimimiz olan 3D baskı kumanda kasası.
* **Adaptif Hız Kontrolü:** Kumanda üzerindeki girdilere duyarlı olarak anlık hız artırıp azaltabilme.
* **Canlı Voltaj Takibi:** Ekranlı regülatör sayesinde anlık batarya voltajı izleme (düşük voltaj/şarj bitti uyarısı).
* **Kablosuz Kontrol:** NRF24L01 modülleri ile kesintisiz ve kararlı sinyalleşme.
* **Görsel Detaylar:** Aracın alt kısmına entegre edilmiş RGB LED'ler.

