

# OTApp - OpenThread Application Framework

![Language](https://img.shields.io/badge/language-Embedded%20C-00599C.svg?style=flat&logo=c)
![Platform](https://img.shields.io/badge/platform-ESP32--C6%20%7C%20ESP--IDF-red.svg?style=flat&logo=espressif&logoColor=white)
![Platform](https://img.shields.io/badge/platform-STM32-03234B.svg?style=flat&logo=stmicroelectronics&logoColor=white)
![Protocol](https://img.shields.io/badge/protocol-OpenThread%20%7C%20CoAP-4caf50.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Tests](https://img.shields.io/badge/tests-Unity%20%7C%20FFF-orange)
![Docs](https://img.shields.io/badge/docs-Doxygen-informational)
![Status](https://img.shields.io/badge/Status-Active_Development-brightgreen.svg?style=flat&logo=github&logoColor=white)
![](https://img.shields.io/badge/Roadmap-New_Devices_%26_Modules-blueviolet?style=flat&logo=gitbook&logoColor=white)


<details><summary><b>🇵🇱 Wersja Polska</b></summary><br>

> **Kompletna dokumentacja techniczna, diagramy oraz opis API dostępne online:**
> 📚 [**https://hareo.pl/otapp/**](https://hareo.pl/otapp/)

## 💡 O Projekcie

**OTApp** to kompleksowy framework middleware stworzony do budowy urządzeń IoT opartych na sieci **Thread**. Stanowi warstwę abstrakcji pomiędzy stosem OpenThread/CoAP a sprzętem, umożliwiając szybkie tworzenie inteligentnych urządzeń, takich jak przyciski, oświetlenie czy czujniki.

Wraz z wersją **v2.0.0**, projekt przekształcił się w dojrzałe, wieloplatformowe rozwiązanie, wprowadzając architekturę pamięci typu zero-copy, ujednolicone API oparte na formacie binarnym TLV oraz dedykowane, sprzętowe sterowniki (np. wykorzystujące GPDMA dla STM32). Projekt rozwiązuje problem skomplikowanej konfiguracji sieci mesh, oferując gotowe mechanizmy **automatycznego parowania (discovery)**, grupowania urządzeń (Device Name Groups) oraz komunikacji asynchronicznej opartych na protokole **CoAP**.

## 🔌 Dostępne Platformy

| MCU            | Toolchain       | Repo         |
|----------------|-----------------|--------------|
| **ESP32-C6** | ESP-IDF v5.x   | **[ github.com/HareoPL/ot_app_esp](https://github.com/HareoPL/ot_app_esp)** |
| **ESP32-S3+ESP32-H2** | ESP-IDF v5.x   | **[ github.com/HareoPL/ot_app_br](https://github.com/HareoPL/ot_app_br)** |
| **STM32WBA65RI**| STM32CubeIDE  | **[github.com/HareoPL/ot_app_stm](https://github.com/HareoPL/ot_app_stm)** |
| **STM32H7+ESP32** | **VSCode+ST HAL** | **[github.com/HareoPL/ot_app_cp](https://github.com/HareoPL/ot_app_cp)** |


## 🛠 Technologie

* **Język:** Embedded C z elementami obiektowości.
* **Platforma:** ESP32-C6 (RISC-V), STM32WBA65RI (Arm® Cortex®-M33).
* **SDK:** ESP-IDF v5.4.1, HAL V1.8.0.
* **Protokoły:** OpenThread (Thread Mesh), CoAP, IPv6.
* **OS:** FreeRTOS.
* **Build System:** CMake / Ninja.
* **Dokumentacja:** Doxygen + Graphviz.

### Kluczowe funkcjonalności:
* ✅ **Zero-Copy & Thread-Safe Buffers (Nowość v2.0):** Zoptymalizowana architektura pamięci współdzielonej eliminująca wyścigi wielowątkowe (race conditions) i drastycznie zmniejszająca narzut RAM.
* ✅ **TLV Messaging API (Nowość v2.0):** Przejście z tekstowych payloadów na wydajny, binarny format Type-Length-Value (TLV) dla zasobów URI i odpowiedzi CoAP, oparty o wydajną strukturę `ot_app_msg_tlv`.
* ✅ **Hardware-Optimized Drivers (Nowość v2.0):** Natywne wsparcie sprzętowe bez obciążania procesora: wydajny sterownik WS2812B wykorzystujący **TIM + GPDMA (oraz przerwania) dla STM32WBA6**, moduł **RMT dla ESP32** oraz polling oparty na taskach RTOS dla przycisków.
* ✅ **Device Abstraction Layer:** Ujednolicone API dla różnych typów urządzeń (Button, Light, Sensor).
* ✅ **Auto-Discovery & Pairing:** Automatyczne wykrywanie i parowanie urządzeń bez udziału użytkownika (Zero-Touch Provisioning).
* ✅ **Pairing Rules & Logic Validation:** System reguł określających dozwolone interakcje między typami urządzeń (whitelist). Framework weryfikuje zgodność urządzeń przed sparowaniem (np. blokuje połączenie `Sensor` <-> `Sensor`), zapewniając logiczną spójność topologii sieci.
* ✅ **Device Grouping & Zoning:** Logiczna segmentacja sieci na strefy sterowania (np. `kitchen`, `living-room`) oparta na prefiksach nazw urządzeń.
* ✅ **CoAP Observe:** Obsługa subskrypcji i notyfikacji zmian stanu w czasie rzeczywistym (RFC 7641).
* ✅ **Architektura zdarzeniowa:** System oparty na callbackach i asynchronicznym przetwarzaniu zadań (Producer-Consumer). Wersja 2.0 automatyzuje to za pomocą wbudowanego taska frameworka.
* ✅ **State Persistence:** Automatyczny zapis konfiguracji sieciowej i tablicy parowania w pamięci nieulotnej (NVS). Gwarantuje przywrócenie pełnej funkcjonalności urządzenia natychmiast po restarcie.
* ✅ **Standard-compliant SRP:** Pełna implementacja klienta **Service Registration Protocol**. Urządzenia dynamicznie rejestrują swoje usługi (`_coap._udp`) w sieci Thread, co zapewnia ich widoczność przez Border Routery.

---

## 📺 Demo i Działanie

System w akcji: automatyczne parowanie urządzenia sterującego (Controller) z oświetleniem (Light Node) oraz reakcja na zdarzenia w czasie rzeczywistym.

![OTApp Demo](https://via.placeholder.com/800x400?text=Place+Your+Demo+GIF+Here)
*Przykład: Po lewej logi z konsoli, po prawej fizyczna reakcja diod LED na wciśnięcie przycisku.*

### 📟 Logi z procesu parowania (ASCII)
Poniższy zrzut logów prezentuje moment automatycznego wykrycia urządzenia w grupie `device1` i wymianę kluczy:

```text
I (4521) [OT-APP]: Device Discovery initiated... Group: "device1"
I (4890) [OT-APP]: Found device: "Light-Node-01" (fe80::1234:5678)
I (4910) [COAP]: Sending Pairing Request...
I (5100) [COAP]: Received ACK. Status: PAIRED.
I (5220) [NVS]: Saving pairing data to flash memory.
> Button Press Detected (Task Polling) -> Action: TOGGLE
I (6500) [COAP-CLI]: Sending PUT /light/state payload: [TLV Data]

```

### 🔄 Przepływ Komunikacji (Sequence Diagram)

Poniższy diagram ilustruje proces parowania oraz mechanizm **CoAP Observe**.
W kroku 8-9 widać wykorzystanie natywnego API OpenThread do wygenerowania 4-bajtowego tokena, który identyfikuje subskrypcję danego zasobu.

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Btn as Button (Controller)
    participant Light as Light Node
    participant Obs as Observer (e.g. Panel)
    
    Note over User, Light: 1. Initialization & Assignment
    User->>Btn: Short Click (Select Slot)
    Btn->>Btn: Mode: Assign Device
    User->>Light: Power On
    Light->>Btn: Report Presence (Discovery)
    
    Note over Btn, Light: 2. Validation
    Btn->>Btn: Check Pairing Rules
    Btn->>Light: GET /.well-known/core (Multicast)
    Light-->>Btn: 2.05 CONTENT (Resource List in TLV)
    
    Note over Btn, Light: 3. Subscription (Pairing)
    Btn->>Btn: OT CoAP API: Generate 4B Token
    Btn->>Light: PUT /... (Option: Observe=0, Token: 0xA1B2C3D4)
    Light-->>Btn: 2.04 CHANGED (ACK)
    Btn->>Btn: Save EUI-64 
    
    Note over User, Obs: 4. Control & Notification Loop
    User->>Btn: Short Click (Action)
    Btn->>Light: PUT /light/state [TLV Payload]
    activate Light
    Light-->>Btn: 2.04 CHANGED (ACK)
    
    Note right of Light: State Updated.<br/>Notify Subscribers.
    
    Light->>Obs: PUT /subscribed_uris {Token: 0xB2.., TLV Data...}
    activate Obs
    Obs-->>Light: 2.04 CHANGED (ACK)
    deactivate Obs
    deactivate Light

```

#### Szczegółowy opis procesu:

1. **Inicjalizacja i przypisanie (Kroki 1-4):**
Użytkownik wprowadza kontroler (Button) w tryb przypisywania dla konkretnego slotu. Włączenie zasilania w nowym urządzeniu (Light) powoduje wysłanie pakietu Discovery/SRP, dzięki czemu kontroler wykrywa obecność nowego węzła w sieci Thread.
2. **Walidacja (Kroki 5-7):**
Kontroler weryfikuje zgodność urządzenia z regułami (np. czy to na pewno lampa). Następnie wysyła zapytanie `GET /.well-known/core` (zgodnie z RFC 6690), aby pobrać listę dostępnych zasobów (URI) na urządzeniu.
3. **Subskrypcja / Parowanie (Kroki 8-11):**
To kluczowy moment parowania.
* Kontroler używa wewnętrznego API OpenThread do wygenerowania unikalnego, **4-bajtowego tokena**.
* Wysyła żądanie `PUT` z opcją **Observe: 0** (Rejestracja).
* Po otrzymaniu potwierdzenia (`ACK`), adres EUI-64 lampy oraz token są trwale zapisywane w pamięci **NVS**. Od teraz przycisk "pamięta" to urządzenie nawet po restarcie.


4. **Kontrola i powiadamianie (Kroki 12-15):**
Podczas normalnej pracy przycisk wysyła komendy sterujące (`PUT`) skompresowane w `ot_app_msg_tlv`. Co istotne, Lampa (Light Node) automatycznie informuje o zmianie stanu wszystkich *innych* obserwatorów (np. panel ścienny), wysyłając asynchroniczne powiadomienie na ich endpoint `/subscribed_uris`. Gwarantuje to synchronizację stanu interfejsów w całym domu.

## 🏗 Architektura i Wzorce Projektowe

Projekt wyróżnia się inżynierskim podejściem do kodu w języku C, implementując paradygmaty **Object-Oriented C** oraz techniki RTOS. Szczegółowa analiza architektury znajduje się w [dokumentacji](https://hareo.pl/otapp/).

### Zastosowane Wzorce Projektowe:

* **Strategy / Interface:** Wykorzystanie wskaźników na funkcje w strukturach sterowników (`ot_app_devDrv_t`) pozwala na wstrzykiwanie zależności (np. reguł parowania) bez modyfikacji rdzenia silnika.
* **Observer:** Luźne powiązanie warstwy sieciowej z logiką aplikacji. Używany do notyfikacji o zmianach topologii sieci oraz obsługi subskrybentów CoAP.
* **Singleton:** Centralizacja zarządzania instancją stosu OpenThread i sterownikami (`otapp_getOpenThreadInstancePtr`).
* **Facade:** Moduł `ot_app_nvs` ukrywa skomplikowane operacje na pamięci Flash, wystawiając prosty interfejs API.
* **Command / Dispatcher:** Obsługa żądań CoAP oparta na tablicy (`otapp_coap_uriDefault`), co eliminuje skomplikowane instrukcje warunkowe.

### Techniki Systemowe i RTOS (FreeRTOS):

* **Model Producer-Consumer:** Oddzielenie kontekstu sieciowego od operacji blokujących. Callbacki sieciowe (Producent) wrzucają zdarzenia do kolejki, które są przetwarzane przez osobny wątek. Wersja 2.0 automatyzuje to za pomocą wbudowanego taska frameworka (`otapp_pair_task`).
* **Zero-Copy & Thread-Safe Buffers:** Wykorzystanie nowego modułu buforów minimalizuje alokacje pamięci i zabezpiecza zasoby (Mutex) przed wyścigami w środowisku wielowątkowym.

## ✅ Jakość Kodu i Testy (QA)

Projekt kładzie duży nacisk na niezawodność i testowalność kodu embedded. Logika aplikacji jest weryfikowana przez **testy jednostkowe uruchamiane na maszynie hosta**.

* **Framework testowy:** [Unity](http://www.throwtheswitch.org/unity)
* **Mockowanie:** [FFF (Fake Function Framework)](https://github.com/meekrosoft/fff) - symulacja warstwy sprzętowej i OpenThread API.
* **Zakres testów:** Kompleksowa weryfikacja logiki parowania (77 testów), obserwatorów CoAP (109 testów), parsowania nazw urządzeń (41 testów) oraz **dokładne testy kalkulacji buforów i współbieżności dla nowego API TLV**.

### **Aby uruchomić testy lokalnie:**

#### Za pomocą CMD:

1. Czyszczenie i konfiguracja CMake

```bash
rm -rf build/unit_test && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE --no-warn-unused-cli -S./tests/unit_test -B./build/unit_test -G Ninja

```

2. Budowanie testów

```bash
cd build/unit_test && ninja -v

```

3. Uruchomienie testów

* ręcznie

```bash
build/unit_test/HOST_ot_app_coap_uri_obs_test/HOST_ot_app_coap_uri_obs_test.exe
build/unit_test/HOST_ot_app_deviceName_test/HOST_ot_app_deviceName_test.exe
build/unit_test/HOST_ot_app_pair_test/HOST_ot_app_pair_test.exe

```

* przy użyciu `CTEST - CMake`

```bash
cd build/unit_test && ctest --output-on-failure -V

```

#### Za pomocą VSC tasks:

* `Ctrl + Shift + P` -> Tasks: Run Task -> `project build`
* `Ctrl + Shift + P` -> Tasks: Run Task -> `test Rebuild Cmake, run test`

## ⚡ Wydajność i Zasoby (Footprint)

Poniższe zestawienie prezentuje rzeczywiste zużycie pamięci dla platformy ESP32-C6 (generowane za pomocą `idf.py size-components`). Zastosowanie w `v2.0.0` architektury Zero-Copy i TLV znacząco obniżyło narzut samej aplikacji.

| Komponent / Warstwa | Flash (Code + RO) | RAM (Static .bss + .data) | Uwagi |
| --- | --- | --- | --- |
| **OpenThread Core (FTD)** | **353 KB** | **~24 KB** | Rola Routera + CLI + Dataset Manager |
| Network & Crypto (LwIP, MbedTLS) | ~250 KB | ~5 KB | Stos TCP/IP, DTLS, Szyfrowanie |
| ESP-IDF Kernel (OS, Drivers, HAL) | ~280 KB | ~35 KB | FreeRTOS, Sterowniki PHY/Radio |
| **OTApp Framework** | **~15 KB** | **< 5 KB** | Narzut drastycznie obniżony po wdrożeniu Zero-Copy |
| User Implementation (e.g. Light) | ~2 KB | ~0.5 KB | Logika biznesowa urządzenia |
| **SUMA (Total Image)** | **~996 KB** | **~72 KB** |  |

## 🧠 Świadomość Techniczna i Roadmapa Refaktoryzacji

Projekt jest aktywnie rozwijany, a dług technologiczny jest sukcesywnie spłacany. Poniższa lista prezentuje status i plan optymalizacji.

### Status Refaktoryzacji:

* [x] **Eliminacja statycznych buforów (Zero-Copy):** ✅ *Wdrożono w v2.0.0. Całkowita przebudowa modułu buforów, eliminująca m.in. statyczny bufor 1KB dla CoAP. Znacznie zmniejszono użycie pamięci RAM z 8 KB do poniżej 5 KB.*
* [x] **Format Binarny Payloadu (CBOR/TLV):** ✅ *Wdrożono w v2.0.0. Pełna integracja API wykorzystująca `ot_app_msg_tlv`, zastępująca niewydajne przesyłanie tekstu JSON.*

### Do zrobienia:

* [ ] **Ujednolicona listy sąsiadów (Unified Neighbor Table):** Zastąpienie niezależnych tablic (`otapp_pair_DeviceList` i `oac_obsSubList`) jedną strukturą współdzieloną `otapp_neighbor_t` zawierającą flagi ról (np. `isPaired`, `isObserver`).
* [ ] **Wdrożenie identyfikacji EUI-64:** Wykorzystanie 8-bajtowego adresu MAC jako unikalnego klucza w logice biznesowej (zamiast parsowania nazw stringami `strtok`, `atoi`).
* [ ] **Migracja NVS na zapis binarny (Binary Blobs):** Użycie `nvs_set_blob` do zapisu surowych struktur C bezpośrednio do Flash, co przyspieszy I/O i zmniejszy zużycie pamięci.
* [ ] **Optymalizacja OpenThread Kconfig:** Redukcja rozmiaru `libopenthread.a` o 50-100 KB poprzez przygotowanie profilu `RELEASE` usuwającego CLI i funkcje Joiner dla środowisk produkcyjnych.
* [ ] **Separacja logów i kanałów IO:** Wdrożenie Log Strippingu za pomocą Kconfig w celu preprocesorowego usunięcia `ESP_LOG` w wersji produkcyjnej.

## 🚀 Instalacja i Uruchomienie

Pełny przewodnik uruchomienia znajduje się w sekcji [Getting Started Guide](https://hareo.pl/otapp/getting_started.html).

### Wymagania sprzętowe

* Minimum 2x ESP32-C6 (np. jedno jako kontroler/przycisk, drugie jako oświetlenie/RGB).
* Środowisko ESP-IDF v5.4.1 lub wyższe.
* Mile widziany Border Router.

### Szybki start

1. **Sklonuj repozytorium:**
```bash
git clone [https://github.com/HareoPL/otapp.git](https://github.com/HareoPL/otapp.git)
cd otapp

```


2. **Zainicjalizuj odpowiednie urządzenie w `main/main.c`:**
```c
void app_main(void)
{
    // ad_button_Init("device1");
    ad_light_init("device1");
    // Hardware abstraction takes care of GPDMA/RMT automatically!
}

```


3. **Skonfiguruj rolę urządzenia:**
Projekt wykorzystuje gotowe fragmenty Kconfig dla różnych ról (Border Router, Controller, Control Panel).
*Dla kontrolera / wykonawcy (button/light):*
```powershell
# W terminalu ESP-IDF
cp components/app_devices/kconfigs/kconfig.controllerORSensor sdkconfig
idf.py reconfigure

```


4. **Zbuduj i wgraj:**
#### CMD:


```bash
idf.py build
idf.py -p COMx flash monitor

```


#### VSC tasks:


* `Ctrl + Shift + P` -> Tasks: Run Task -> `project build`
* `Ctrl + Shift + P` -> Tasks: Run Task -> `project flash` (pamiętaj o poprawnie wybranym porcie COM)



## 🗺 Roadmap

Plany rozwoju projektu:

* [x] Implementacja core frameworka na ESP32-C6 (ESP-IDF)
* [x] Obsługa CoAP Observe (RFC 7641) oraz Auto-Discovery
* [x] **Przebudowa architektury (TLV API, Zero-Copy Buffers)** (Wydanie v2.0.0)
* [x] Port na platformę STM32 (Nucleo/STM32WB/WBA)
</BR> 👉 zrealizowane - STM32WBA65RI z GPDMA: **[github.com/HareoPL/ot_app_stm](https://github.com/HareoPL/ot_app_stm)**
* [ ] **Implementacja urządzenia "OpenThread UART Bridge" (Serial-to-CoAP/Thread)**
* [ ] Implementacja "Control Panel" (STM32H7 + EXTERNAL RAM + EXTERNAL FLASH + ESP32-C6 + LCD + TouchGFX) jako centralny punkt sterowania.
</BR> 🛠👉 w trakcie: **[github.com/HareoPL/ot_app_cp](https://github.com/HareoPL/ot_app_cp)**
* [ ] Aplikacja PC do zarządzania siecią przez mostek UART.

## 👨‍💻 Autor i Kontakt

**Jan Łukaszewicz**

* 📧 E-mail: plhareo@gmail.com
* 🔗 www: [hareo.pl](https://hareo.pl/)

Projekt udostępniany na licencji MIT.

</details>

## 🇺🇸 English

> **Full technical documentation, diagrams, and API reference available online:**
> 📚 **[https://hareo.pl/otapp/](https://hareo.pl/otapp/)**

## 💡 About the Project

**OTApp** is a comprehensive middleware framework designed for building IoT devices based on the **Thread** network. It serves as an abstraction layer between the OpenThread/CoAP stack and the hardware, enabling rapid development of smart devices such as buttons, lighting, or sensors.

With the release of **v2.0.0**, the project has evolved into a mature, multi-platform solution, introducing a zero-copy memory architecture, a unified binary TLV API, and dedicated hardware-optimized drivers (e.g., utilizing GPDMA for STM32). The project addresses the complexity of mesh network configuration by offering out-of-the-box mechanisms for **automatic pairing (discovery)**, device grouping (Device Name Groups), and asynchronous communication based on the **CoAP** protocol.

## 🔌 Available Platforms

| MCU | Toolchain | Repo |
| --- | --- | --- |
| **ESP32-C6** | ESP-IDF v5.x | **[ github.com/HareoPL/ot_app_esp](https://github.com/HareoPL/ot_app_esp)** |
| **ESP32-S3+ESP32-H2** | ESP-IDF v5.x | **[ github.com/HareoPL/ot_app_br](https://github.com/HareoPL/ot_app_br)** |
| **STM32WBA65RI** | STM32CubeIDE | **[github.com/HareoPL/ot_app_stm](https://github.com/HareoPL/ot_app_stm)** |
| **STM32H7+ESP32** | **VSCode+ST HAL** | **[github.com/HareoPL/ot_app_cp](https://github.com/HareoPL/ot_app_cp)** |

## 🛠 Technologies

* **Language:** Embedded C with object-oriented patterns.
* **Platforms:** ESP32-C6 (RISC-V), STM32WBA65RI (Arm® Cortex®-M33).
* **SDK:** ESP-IDF v5.4.1, HAL V1.8.0.
* **Protocols:** OpenThread (Thread Mesh), CoAP, IPv6.
* **OS:** FreeRTOS.
* **Build System:** CMake / Ninja.
* **Documentation:** Doxygen + Graphviz.

### Key Features:

* ✅ **Zero-Copy & Thread-Safe Buffers (New in v2.0):** Optimized shared memory architecture eliminating race conditions and drastically reducing RAM overhead.
* ✅ **TLV Messaging API (New in v2.0):** Transitioned from text payloads to an efficient, binary Type-Length-Value (TLV) format for CoAP resources, relying on the `ot_app_msg_tlv` structure.
* ✅ **Hardware-Optimized Drivers (New in v2.0):** Native hardware offloading: highly optimized WS2812B driver utilizing **TIM + GPDMA (with interrupts) for STM32WBA6**, **RMT module for ESP32**, and RTOS task polling for buttons.
* ✅ **Device Abstraction Layer:** Unified API for various device types (Button, Light, Sensor).
* ✅ **Auto-Discovery & Pairing:** Automatic device detection and pairing without user intervention (Zero-Touch Provisioning).
* ✅ **Pairing Rules & Logic Validation:** A rule-based system defining allowed interactions between device types (whitelist). The framework verifies device compatibility before pairing (e.g., blocking `Sensor` <-> `Sensor` connections), ensuring logical network topology.
* ✅ **Device Grouping & Zoning:** Logical network segmentation into control zones (e.g., `kitchen`, `living-room`) based on device name prefixes.
* ✅ **CoAP Observe:** Support for real-time state subscriptions and notifications (RFC 7641).
* ✅ **Event-Driven Architecture:** System based on callbacks and asynchronous task processing (Producer-Consumer). In v2.0, this is fully managed by an internal framework task.
* ✅ **State Persistence:** Automatic storage of network configuration and pairing tables in Non-Volatile Storage (NVS). Guarantees full functionality restoration immediately after reboot.
* ✅ **Standard-compliant SRP:** Full implementation of the **Service Registration Protocol** client. Devices dynamically register their services (`_coap._udp`) within the Thread network, ensuring visibility to Border Routers.

---

## 📺 Demo & Operation

System in action: automatic pairing of a Controller with a Light Node and real-time event response.

*Example: Console logs on the left, physical LED reaction to a button press on the right.*

### 📟 Pairing Process Logs (ASCII)

The log snippet below demonstrates the moment of automatic device discovery in the `device1` group and the subsequent key exchange:

```text
I (4521) [OT-APP]: Device Discovery initiated... Group: "device1"
I (4890) [OT-APP]: Found device: "Light-Node-01" (fe80::1234:5678)
I (4910) [COAP]: Sending Pairing Request...
I (5100) [COAP]: Received ACK. Status: PAIRED.
I (5220) [NVS]: Saving pairing data to flash memory.
> Button Press Detected (Task Polling) -> Action: TOGGLE
I (6500) [COAP-CLI]: Sending PUT /light/state payload: [TLV Data]


```

### 🔄 Communication Flow (Sequence Diagram)

The following diagram illustrates the pairing process and the **CoAP Observe** mechanism. Steps 8-9 highlight the use of the native OpenThread API to generate a 4-byte token identifying the resource subscription.

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Btn as Button (Controller)
    participant Light as Light Node
    participant Obs as Observer (e.g. Panel)
    
    Note over User, Light: 1. Initialization & Assignment
    User->>Btn: Short Click (Select Slot)
    Btn->>Btn: Mode: Assign Device
    User->>Light: Power On
    Light->>Btn: Report Presence (Discovery)
    
    Note over Btn, Light: 2. Validation
    Btn->>Btn: Check Pairing Rules
    Btn->>Light: GET /.well-known/core (Multicast)
    Light-->>Btn: 2.05 CONTENT (Resource List in TLV)
    
    Note over Btn, Light: 3. Subscription (Pairing)
    Btn->>Btn: OT CoAP API: Generate 4B Token
    Btn->>Light: PUT /... (Option: Observe=0, Token: 0xA1B2C3D4)
    Light-->>Btn: 2.04 CHANGED (ACK)
    Btn->>Btn: Save EUI-64 
    
    Note over User, Obs: 4. Control & Notification Loop
    User->>Btn: Short Click (Action)
    Btn->>Light: PUT /light/state [TLV Payload]
    activate Light
    Light-->>Btn: 2.04 CHANGED (ACK)
    
    Note right of Light: State Updated.<br/>Notify Subscribers.
    
    Light->>Obs: PUT /subscribed_uris {Token: 0xB2.., TLV Data...}
    activate Obs
    Obs-->>Light: 2.04 CHANGED (ACK)
    deactivate Obs
    deactivate Light


```

#### Detailed Process Description:

1. **Initialization & Assignment (Steps 1-4):**
The user puts the controller (Button) into assignment mode for a specific slot. Powering on a new device (Light) triggers a Discovery/SRP packet, allowing the controller to detect the new node on the Thread network.
2. **Validation (Steps 5-7):**
The controller verifies device compatibility against rules (e.g., ensuring it is indeed a light). It then sends a `GET /.well-known/core` request (per RFC 6690) to retrieve the list of available resources (URIs) on the device.
3. **Subscription / Pairing (Steps 8-11):**
This is the critical pairing moment:

* The controller uses the internal OpenThread API to generate a unique **4-byte token**.
* It sends a `PUT` request with the **Observe: 0** option (Registration).
* Upon receiving an acknowledgment (`ACK`), the light's EUI-64 address and the token are permanently stored in **NVS**. The button now "remembers" this device even after a reboot.

4. **Control & Notification (Steps 12-15):**
During normal operation, the button sends control commands (`PUT`) compressed inside the `ot_app_msg_tlv` format. Crucially, the Light Node automatically informs all *other* observers (e.g., a wall panel) of state changes by sending an asynchronous notification to their `/subscribed_uris` endpoint. This ensures UI state synchronization across the entire home.

## 🏗 Architecture & Design Patterns

The project stands out for its engineering approach to C code, implementing **Object-Oriented C** paradigms and RTOS techniques. A detailed architectural analysis is available in the [documentation](https://hareo.pl/otapp/).

### Applied Design Patterns:

* **Strategy / Interface:** Function pointers in driver structures (`ot_app_devDrv_t`) allow dependency injection (e.g., pairing rules) without modifying the core engine.
* **Observer:** Loose coupling between the network layer and application logic. Used for network topology notifications and CoAP subscriber handling.
* **Singleton:** Centralized management of the OpenThread stack instance and drivers (`otapp_getOpenThreadInstancePtr`).
* **Facade:** The `ot_app_nvs` module hides complex Flash memory operations, exposing a simplified API.
* **Command / Dispatcher:** CoAP request handling based on an table (`otapp_coap_uriDefault`), eliminating complex conditional statements.

### System Techniques & RTOS (FreeRTOS):

* **Producer-Consumer Model:** Decoupling network context from blocking operations. In v2.0, this is fully automated by the internal framework task (`otapp_pair_task`).
* **Zero-Copy & Thread-Safe Buffers:** The new buffer module minimizes memory allocations and protects resources (Mutexes) against race conditions in a multi-threaded environment.

## ✅ Code Quality & Testing (QA)

The project emphasizes reliability and testability. Application logic (pairing, URI handling, name parsing) is verified via **unit tests running on the host machine**.

* **Testing Framework:** [Unity](http://www.throwtheswitch.org/unity)
* **Mocking:** [FFF (Fake Function Framework)](https://github.com/meekrosoft/fff) - simulating hardware and OpenThread API.
* **Test Coverage:** Comprehensive validation of device pairing logic (77 Tests), CoAP observers (109 Tests), name parsing (41 Tests), and **rigorous buffer calculation/concurrency testing for the new TLV API**.

### **Running tests locally:**

#### Via CMD:

1. Clean and configure CMake:

```bash
rm -rf build/unit_test && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE --no-warn-unused-cli -S./tests/unit_test -B./build/unit_test -G Ninja

```

2. Build tests:

```bash
cd build/unit_test && ninja -v

```

3. Execute tests:

* manually

```bash
build/unit_test/HOST_ot_app_coap_uri_obs_test/HOST_ot_app_coap_uri_obs_test.exe
build/unit_test/HOST_ot_app_deviceName_test/HOST_ot_app_deviceName_test.exe
build/unit_test/HOST_ot_app_pair_test/HOST_ot_app_pair_test.exe

```

* using `CTEST - CMake`

```bash
cd build/unit_test && ctest --output-on-failure -V

```

#### Via VSC tasks:

* `Ctrl + Shift + P` -> Tasks: Run Task -> `project build`
* `Ctrl + Shift + P` -> Tasks: Run Task -> `test Rebuild Cmake, run test`

## ⚡ Performance & Footprint

The following table presents actual memory consumption for the ESP32-C6 platform, generated using the `idf.py size-components` tool. Implementing the Zero-Copy and TLV architecture in `v2.0.0` has significantly reduced the application's base overhead.

| Component / Layer | Flash (Code + RO) | RAM (Static .bss + .data) | Notes |
| --- | --- | --- | --- |
| **OpenThread Core (FTD)** | **353 KB** | **~24 KB** | Router Role + CLI + Dataset Manager |
| Network & Crypto (LwIP, MbedTLS) | ~250 KB | ~5 KB | TCP/IP Stack, DTLS, Encryption |
| ESP-IDF Kernel (OS, Drivers, HAL) | ~280 KB | ~35 KB | FreeRTOS, PHY/Radio Drivers |
| **OTApp Framework** | **~15 KB** | **< 5 KB** | Overhead drastically reduced by Zero-Copy |
| User Implementation (e.g. Light) | ~2 KB | ~0.5 KB | Device business logic |
| **TOTAL (Image)** | **~996 KB** | **~72 KB** |  |

## 🧠 Technical Awareness & Refactoring Roadmap

While functional, the project has identified areas requiring optimization for production environments. This list constitutes the Technical Debt and remediation schedule.

### Refactoring Status:

* [x] **Zero-Copy Implementation:** ✅ *Delivered in v2.0.0. Complete overhaul of the buffer module, eliminating static 1KB allocations. Reduced RAM overhead from 8 KB to under 5 KB.*
* [x] **Binary Payload Format (CBOR/TLV):** ✅ *Delivered in v2.0.0. Full integration of the `ot_app_msg_tlv` API, replacing inefficient JSON text parsing.*

### To Do:

* [ ] **Unified Neighbor Table:** Replace independent arrays (`otapp_pair_DeviceList` and `oac_obsSubList`) with a single shared structure `otapp_neighbor_t` containing role flags (e.g., `isPaired`, `isObserver`).
* [ ] **EUI-64 Identification:** Use the 8-byte MAC address as the unique key in business logic (instead of string parsing via `strtok`, `atoi`).
* [ ] **Migration to Binary Blobs (NVS):** Use `nvs_set_blob` to write raw C structures directly to Flash. This will accelerate I/O and reduce memory footprint.
* [ ] **OpenThread Kconfig Optimization:** Prepare a `RELEASE` profile in `sdkconfig` that disables CLI and Joiner functions to reclaim 50-100 KB of Flash.
* [ ] **Logging & IO Separation:** Add a `Kconfig` option to pre-process out `ESP_LOG` in production builds.

## 🚀 Installation & Setup

A full setup guide is available in the [Getting Started Guide](https://hareo.pl/otapp/getting_started.html).

### Hardware Requirements

* Minimum 2x ESP32-C6 (e.g., one as a controller/button, one as light/RGB).
* ESP-IDF v5.4.1 environment or higher.
* A Border Router is recommended.

### Quick Start

1. **Clone the repository:**

```bash
git clone [https://github.com/HareoPL/otapp.git](https://github.com/HareoPL/otapp.git)
cd otapp

```

2. **Initialize the device in `main/main.c`:**

```c
void app_main(void)
{
  // ad_button_Init("device1");
  ad_light_init("device1");
  // Hardware abstraction takes care of GPDMA/RMT automatically!
}

```

3. **Configure Device Role:**
The project uses Kconfig fragments for different roles.
*For Controller/Actuator (button/light):*

```powershell
# In ESP-IDF terminal
cp components/app_devices/kconfigs/kconfig.controllerORSensor sdkconfig
idf.py reconfigure

```

4. **Build and Flash:**

#### CMD:

```bash
idf.py build
idf.py -p COMx flash monitor

```

#### VSC tasks:

* `Ctrl + Shift + P` -> Tasks: Run Task -> `project build`
* `Ctrl + Shift + P` -> Tasks: Run Task -> `project flash` (ensure correct COM port)

## 🗺 Roadmap

Development plans:

* [x] Core framework implementation on ESP32-C6 (ESP-IDF)
* [x] CoAP Observe support (RFC 7641) & Auto-Discovery
* [x] **Architecture Overhaul (TLV API, Zero-Copy Buffers)** (Release v2.0.0)
* [x] Port to STM32 platform (Nucleo/STM32WB/WBA)
</BR> 👉 completed - STM32WBA65RI with GPDMA: **[github.com/HareoPL/ot_app_stm](https://github.com/HareoPL/ot_app_stm)**
* [ ] **"OpenThread UART Bridge" (Serial-to-CoAP/Thread) implementation**
* [ ] "Control Panel" implementation (STM32H7 + EXTERNAL RAM + EXTERNAL FLASH + ESP32-C6 + LCD + TouchGFX) as a central hub.
</BR> 🛠👉 in progress: **[github.com/HareoPL/ot_app_cp](https://github.com/HareoPL/ot_app_cp)**
* [ ] PC Application for network management via UART bridge.

## 👨‍💻 Author & Contact

**Jan Łukaszewicz**

* 📧 E-mail: plhareo@gmail.com
* 🔗 www: [hareo.pl](https://hareo.pl/)

This project is licensed under the MIT License.
