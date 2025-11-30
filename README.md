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

> **Kompletna dokumentacja techniczna, diagramy oraz opis API dostępne online:**
> 📚 [**https://hareo.pl/otapp/**](https://hareo.pl/otapp/)

## 💡 O Projekcie

**OTApp** to kompleksowy framework middleware stworzony do budowy urządzeń IoT opartych na sieci **Thread**. Stanowi warstwę abstrakcji pomiędzy stosem OpenThread/CoAP a sprzętem, umożliwiając szybkie tworzenie inteligentnych urządzeń, takich jak przyciski, oświetlenie czy czujniki.

Projekt rozwiązuje problem skomplikowanej konfiguracji sieci mesh, oferując gotowe mechanizmy **automatycznego parowania (discovery)**, grupowania urządzeń (Device Name Groups) oraz komunikacji asynchronicznej opartej na protokole **CoAP**.

## 🛠 Technologie

* **Język:** Embedded C z elementami obiektowości.
* **Platforma:** ESP32-C6 (RISC-V), STM32WBA65RI (Arm® Cortex®-M33).
* **SDK:** ESP-IDF v5.4.1, HAL V1.8.0.
* **Protokoły:** OpenThread (Thread Mesh), CoAP, IPv6.
* **OS:** FreeRTOS.
* **Build System:** CMake / Ninja.
* **Dokumentacja:** Doxygen + Graphviz.
### Kluczowe funkcjonalności:
* ✅ **Device Abstraction Layer:** Ujednolicone API dla różnych typów urządzeń (Button, Light, Sensor).
* ✅ **Auto-Discovery & Pairing:** Automatyczne wykrywanie i parowanie urządzeń bez udziału użytkownika (Zero-Touch Provisioning).
* ✅ **Pairing Rules & Logic Validation:** System reguł określających dozwolone interakcje między typami urządzeń (whitelist). Framework weryfikuje zgodność urzadzen przed sparowaniem (np. blokuje połączenie `Sensor` <-> `Sensor`), zapewniając logiczną spójność topologii sieci.
* ✅ **Device Grouping & Zoning:** Logiczna segmentacja sieci na strefy sterowania (np. `kitchen`, `living-room`) oparta na prefiksach nazw urządzeń.
* ✅ **CoAP Observe:** Obsługa subskrypcji i notyfikacji zmian stanu w czasie rzeczywistym (RFC 7641).
* ✅ **Architektura zdarzeniowa:** System oparty na callbackach i asynchronicznym przetwarzaniu zadań (Producer-Consumer).
* ✅ **State Persistence:** Automatyczny zapis konfiguracji sieciowej i tablicy parowania w pamięci nieulotnej (NVS). Gwarantuje przywrócenie pełnej funkcjonalności urządzenia natychmiast po restarcie.
* ✅ **Standard-compliant SRP:** Pełna implementacja klienta **Service Registration Protocol**. Urządzenia dynamicznie rejestrują swoje usługi (`_coap._udp`) w sieci Thread, co zapewnia ich widoczność przez Border Routery.

---

## 📺 Demo i Działanie

System w akcji: automatyczne parowanie urządzenia sterującego (Controller) z oświetleniem (Light Node) oraz reakcja na zdarzenia w czasie rzeczywistym.

![OTApp Demo](https://via.placeholder.com/800x400?text=Place+Your+Demo+GIF+Here)
*Przykład: Po lewej logi z konsoli, po prawej fizyczna reakcja diod LED na wciśnięcie przycisku.*

### 📟 Logi z procesu parowania (ASCII Cinema)
Poniższy zrzut logów prezentuje moment automatycznego wykrycia urządzenia w grupie `device1` i wymianę kluczy:

```text
I (4521) [OT-APP]: Device Discovery initiated... Group: "device1"
I (4890) [OT-APP]: Found device: "Light-Node-01" (fe80::1234:5678)
I (4910) [COAP]: Sending Pairing Request...
I (5100) [COAP]: Received ACK. Status: PAIRED.
I (5220) [NVS]: Saving pairing data to flash memory.
> Button Press Detected (GPIO 3) -> Action: TOGGLE
I (6500) [COAP-CLI]: Sending PUT /light/state payload: {on: true}
````

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
    Light-->>Btn: 2.05 CONTENT (Resource List)
    
    Note over Btn, Light: 3. Subscription (Pairing)
    Btn->>Btn: OT CoAP API: Generate 4B Token
    Btn->>Light: PUT /... (Option: Observe=0, Token: 0xA1B2C3D4)
    Light-->>Btn: 2.04 CHANGED (ACK)
    Btn->>Btn: Save EUI-64 
    
    Note over User, Obs: 4. Control & Notification Loop
    User->>Btn: Short Click (Action)
    Btn->>Light: PUT /light/state {"on": true}
    activate Light
    Light-->>Btn: 2.04 CHANGED (ACK)
    
    Note right of Light: State Updated.<br/>Notify Subscribers.
    
    Light->>Obs: PUT /subscribed_uris {Token: 0xB2.., Data...}
    activate Obs
    Obs-->>Light: 2.04 CHANGED (ACK)
    deactivate Obs
    deactivate Light
```

#### Szczegółowy opis procesu:

1.  **Initialization & Assignment (Kroki 1-4):**
    Użytkownik wprowadza kontroler (Button) w tryb przypisywania dla konkretnego slotu. Włączenie zasilania w nowym urządzeniu (Light) powoduje wysłanie pakietu Discovery/SRP, dzięki czemu kontroler wykrywa obecność nowego węzła w sieci Thread.

2.  **Validation (Kroki 5-7):**
    Kontroler weryfikuje zgodność urządzenia z regułami (np. czy to na pewno lampa). Następnie wysyła zapytanie `GET /.well-known/core` (zgodnie z RFC 6690), aby pobrać listę dostępnych zasobów (URI) na urządzeniu.

3.  **Subscription / Pairing (Kroki 8-11):**
    To kluczowy moment parowania.
      * Kontroler używa wewnętrznego API OpenThread do wygenerowania unikalnego, **4-bajtowego tokena**.
      * Wysyła żądanie `PUT` z opcją **Observe: 0** (Rejestracja).
      * Po otrzymaniu potwierdzenia (`ACK`), adres EUI-64 lampy oraz token są trwale zapisywane w pamięci **NVS**. Od teraz przycisk "pamięta" to urządzenie nawet po restarcie.

4.  **Control & Notification (Kroki 12-15):**
    Podczas normalnej pracy przycisk wysyła komendy sterujące (`PUT`). Co istotne, Lampa (Light Node) automatycznie informuje o zmianie stanu wszystkich *innych* obserwatorów (np. panel ścienny), wysyłając asynchroniczne powiadomienie na ich endpoint `/subscribed_uris`. Gwarantuje to synchronizację stanu interfejsów w całym domu.

## 🏗 Architektura i Wzorce Projektowe

Projekt wyróżnia się inżynierskim podejściem do kodu w języku C, implementując paradygmaty **Object-Oriented C** oraz zaawansowane techniki RTOS. Szczegółowa analiza architektury znajduje się w [dokumentacji](https://hareo.pl/otapp/).
### Zastosowane Wzorce Projektowe (GoF):
  * **Strategy / Interface:** Wykorzystanie wskaźników na funkcje w strukturach sterowników (`ot_app_devDrv_t`) pozwala na wstrzykiwanie zależności (np. reguł parowania) bez modyfikacji rdzenia silnika.
  * **Observer:** Luźne powiązanie warstwy sieciowej z logiką aplikacji. Używany do notyfikacji o zmianach topologii sieci oraz obsługi subskrybentów CoAP.
  * **Singleton:** Centralizacja zarządzania instancją stosu OpenThread i sterownikami (`otapp_getOpenThreadInstancePtr`).
  * **Facade:** Moduł `ot_app_nvs` ukrywa skomplikowane operacje na pamięci Flash, wystawiając prosty interfejs API.
  * **Command / Dispatcher:** Obsługa żądań CoAP oparta na tablicy asocjacyjnej (`otapp_coap_uriDefault`), co eliminuje skomplikowane instrukcje warunkowe.

### Techniki Systemowe i RTOS (FreeRTOS):
  * **Model Producer-Consumer:** Oddzielenie kontekstu sieciowego od operacji blokujących. Callbacki sieciowe (Producent) wrzucają zdarzenia do kolejki, które są przetwarzane przez osobny wątek (Konsument - `otapp_pair_task`).
  * **Ochrona zasobów (Mutex):** Zabezpieczenie współdzielonych buforów pamięci przed wyścigami (race conditions) w środowisku wielowątkowym.

## ✅ Jakość Kodu i Testy (QA)

Projekt kładzie duży nacisk na niezawodność i testowalność kodu embedded. Logika aplikacji (parowanie, obsługa URI, parsowanie nazw) jest weryfikowana przez **testy jednostkowe uruchamiane na maszynie hosta**.

  * **Framework testowy:** [Unity](http://www.throwtheswitch.org/unity)
  * **Mockowanie:** [FFF (Fake Function Framework)](https://github.com/meekrosoft/fff) - symulacja warstwy sprzętowej i OpenThread API.
  * **Zakres testów:**
      * `ot_app_pair_test.c` – kompleksowa weryfikacja logiki parowania urządzeń.
      * `ot_app_pair_rtos_test.c` – testy integracji mechanizmu parowania z RTOS-em.
      * `ot_app_coap_uri_obs_test.c` – testy mechanizmu obserwatorów CoAP i subskrypcji.
      * `ot_app_deviceName_test.c` – walidacja parsowania nazw i grup urządzeń.

### **Aby uruchomić testy lokalnie:**
#### Za pomoca CMD:
1. Czyszczenie i konfiguracja CMake
```bash
rm -rf build/unit_test && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE --no-warn-unused-cli -S./tests/unit_test -B./build/unit_test -G Ninja
```
2. Budowanie testów
```bash
cd build/unit_test && ninja -v
```
3. Uruchomienie testów
```bash
HOST_ot_app_coap_uri_obs_test/HOST_ot_app_coap_uri_obs_test.exe
HOST_ot_app_deviceName_test/HOST_ot_app_deviceName_test.exe
HOST_ot_app_pair_test/HOST_ot_app_pair_test.exe
```

#### Za pomoca VSC tasks:
- ctr + shift + p -> Tasks: run Task -> project build
- ctr + shift + p -> Tasks: run Task -> test Rebuild Cmake, run test


## ⚡ Wydajność i Zasoby (Footprint)

Poniższe zestawienie prezentuje rzeczywiste zużycie pamięci dla platformy ESP32-C6, wygenerowane za pomocą narzędzia analitycznego `idf.py size-components`.

Projekt charakteryzuje się lekkim narzutem własnym frameworka (OTApp), przy czym główną część zasobów konsumuje stos sieciowy OpenThread w pełnej konfiguracji FTD (Full Thread Device) oraz kernel systemu.

| Komponent / Warstwa               | Flash (Code + RO) | RAM (Static .bss + .data) | Uwagi                                     |
| :-------------------------------- | :---------------- | :------------------------ | :---------------------------------------- |
| **OpenThread Core (FTD)**         | **353 KB**        | **\~24 KB**               | Rola Routera + CLI + Dataset Manager      |
| Network & Crypto (LwIP, MbedTLS)  | \~250 KB          | \~5 KB                    | Stos TCP/IP, DTLS, Szyfrowanie            |
| ESP-IDF Kernel (OS, Drivers, HAL) | \~280 KB          | \~35 KB                   | FreeRTOS, Sterowniki PHY/Radio            |
| **OTApp Framework**               | **\~12 KB**       | **\~8 KB**                | Middleware (Główny cel optymalizacji RAM) |
| User Implementation (e.g. Light)  | \~2 KB            | \~0.5 KB                  | Logika biznesowa urządzenia               |
| **SUMA (Total Image)**            | **\~996 KB**      | **\~72 KB**               |                                           |

> **Wnioski z analizy:**
>
>   * Moduł `libot_app.a` zajmuje **\~8 KB RAM** w sekcji `.bss` (zmienne statyczne). Jest to wynik statycznej alokacji buforów (m.in. bufor CoAP 1KB oraz tablice sąsiadów).
>   * Planowana refaktoryzacja (patrz sekcja *Roadmap*) ma na celu redukcję tego narzutu o ok. 50-60%.

## 🧠 Świadomość Techniczna i Roadmapa Refaktoryzacji

Projekt, choć funkcjonalny, posiada zidentyfikowane obszary, które w środowisku produkcyjnym wymagają optymalizacji. Poniższa lista stanowi plan długu technicznego (Technical Debt) i harmonogram prac naprawczych.

### 1\. Architektura Danych (RAM Optimization)
  * **Problem:** Rozdzielenie logiki na dwie niezależne listy (`otapp_pair_DeviceList` i `oac_obsSubList`) powoduje redundancję danych i zużycie RAM.
  * **Plan naprawczy:**
      - [ ] **Ujednolicona listy sąsiadów (Unified Neighbor Table):** Zastąpienie niezależnych tablic jedną strukturą `otapp_neighbor_t` zawierającą flagi ról (np. `isPaired`, `isObserver`).
      - [ ] **Optymalizacja flag bitowych:** Zastąpienie tablic `uint8_t` bitmaskami w celu szybszej iteracji.

### 2\. Pamięć Trwała (NVS & Storage)
  * **Problem:** Konfiguracja zapisywana jest jako stringi (`nvs_set_str`), co jest nieefektywne i wymaga parsowania.
  * **Plan naprawczy:**
      - [ ] **Migracja na zapis binarny (Binary Blobs):** Użycie `nvs_set_blob` do zapisu surowych struktur C bezpośrednio do Flash. Przyspieszy to I/O i zmniejszy zużycie pamięci.

### 3\. Wydajność i Logika (Core Logic)
  * **Problem:** Identyfikacja urządzeń oparta na operacjach na stringach (`strtok`, `atoi`) oraz przesyłanie payloadu JSON/Text.
  * **Plan naprawczy:**
      - [ ] **Wdrożenie identyfikacji EUI-64:** Wykorzystanie 8-bajtowego adresu MAC jako unikalnego klucza w logice biznesowej (zamiast parsowania nazw).
      - [ ] **Implementacja CBOR (RFC 8949):** Migracja payloadu CoAP na format binarny (TinyCBOR), co zmniejszy fragmentację pakietów.

### 4\. Pamięć RAM (Buffer Management)
  * **Problem:** Statyczna alokacja dużych buforów (np. 1KB w `ot_app_coap_uri.c`).
  * **Plan naprawczy:**
      - [ ] **Eliminacja statycznych buforów (Zero-Copy):** Implementacja czytania danych z `otMessage` bezpośrednio do struktur docelowych lub dynamiczna alokacja małych buforów na stosie.

### 5\. Optymalizacja Konfiguracji Stosu (Kconfig Tuning)
  * **Problem:** `libopenthread.a` zajmuje \~353 KB (35% firmware'u).
  * **Plan naprawczy:**
      - [ ] **Redukcja rozmiaru OpenThread:** Przygotowanie profilu `RELEASE` w `sdkconfig`, wyłączającego CLI, rozbudowane logowanie i funkcje Joiner, co pozwoli odzyskać 50-100 KB Flash.

### 6\. System Logowania i Komunikacja (Logging & IO)
  * **Problem:** Logi mieszają się z danymi na UART i zajmują sekcję `.rodata`.
  * **Plan naprawczy:**
      - [ ] **Log Stripping:** Dodanie opcji w `Kconfig` do preprocesorowego usunięcia `ESP_LOG` w wersji produkcyjnej.
      - [ ] **Separacja kanałów:** Przekierowanie logów na USB-JTAG, aby dedykować główny UART dla mostka OpenThread Bridge.

## 🚀 Instalacja i Uruchomienie

Pełny przewodnik uruchomienia znajduje się w sekcji [Getting Started Guide](https://hareo.pl/otapp/getting_started.html).

### Wymagania sprzętowe
  * Minimum 2x ESP32-C6 (np. jedno jako kontroler/przycisk, drugie jako oświetlenie/RGB).
  * Środowisko ESP-IDF v5.4.1.
  * Mile widziany Border Router.

### Szybki start
1.  **Sklonuj repozytorium:**

    ```bash
    git clone [https://github.com/twoj-nick/otapp.git](https://github.com/twoj-nick/otapp.git)
    cd otapp
    ```

2.  2.  **W main/ main.c wybierz odpowiednia inicjalizacje urzadzenia (Light lub button device):**
 
  ```C
void app_main(void)
{
	// /////////////////////
	// add device init here
	
	// ad_button_Init("device1");
	ad_light_init("device1");
	
	// DO NOT EDIT BELOW //
	// ///////////////////
	
	// ...
}
   ```
3.  **Skonfiguruj rolę urządzenia:**
    Projekt wykorzystuje gotowe fragmenty Kconfig dla różnych ról (Border Router, Controller, Control Panel).

    *Dla kontrolera / wykonawcy (button/light):*

    ```powershell
    # W terminalu ESP-IDF
    cp components/app_devices/kconfigs/kconfig.controllerORSensor sdkconfig
    idf.py reconfigure
    ```

4.  **Zbuduj i wgraj:**
    #### CMD:
    ```bash
    idf.py build
    idf.py -p COMx flash monitor
    ```
    #### VSC tasks:
    - ctr + shift + p -> Tasks: run Task -> project build
    - ctr + shift + p -> Tasks: run Task -> project flash (pamietaj o poprawnie wybranym porcie COM)

## 🗺 Roadmap

Plany rozwoju projektu:

  * [x] Implementacja core frameworka na ESP32-C6 (ESP-IDF)
  * [x] Obsługa CoAP Observe (RFC 7641)
  * [x] Automatyczne parowanie (Discovery) i grupowanie
  * [ ] **Implementacja urządzenia "OpenThread UART Bridge" (Serial-to-CoAP/Thread)**
  * [ ] Port na platformę STM32 (Nucleo/STM32WB)
  * [ ] Implementacja "Control Panel" (STM32H7/ESP32-C6 + LCD + TouchGFX) jako centralny punkt sterowania.
  * [ ] Aplikacja PC do zarządzania siecią przez mostek UART.

## 👨‍💻 Autor i Kontakt

**Jan Łukaszewicz**

  * 📧 E-mail: pldevluk@gmail.com
  * 🔗 Dokumentacja: [hareo.pl/otapp](https://hareo.pl/otapp/)

Projekt udostępniany na licencji MIT.