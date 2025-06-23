# ARP Spoofing Tool - C++

Nowoczesna implementacja narzędzia ARP spoofing napisana w C++17, wykorzystująca inteligentne wskaźniki, atomiki, callbacki i wzorce projektowe. Program jest wieloplatformowy i nie wymaga zewnętrznych zależności.

## 🎯 **Pojęcia kluczowe**

### **Ofiara (Victim)**
- To host, który **nie wie** że jest atakowany
- Jego ruch sieciowy jest przechwytywany i przekierowywany
- Myśli, że komunikuje się bezpośrednio z celem
- Przykład: komputer użytkownika w sieci

### **Cel (Target)**
- To host, z którym ofiara próbuje się komunikować
- Może być bramą domyślną, serwerem, innym komputerem
- Przykład: router, serwer internetowy, inny komputer w sieci

### **Tryb jednokierunkowy (One-way mode)**
- **Normalny tryb:** Oszukujemy zarówno ofiarę jak i cel
  - Ofiara myśli, że nasz komputer to cel
  - Cel myśli, że nasz komputer to ofiara
  - Przechwytujemy ruch w obu kierunkach

- **Tryb jednokierunkowy:** Oszukujemy tylko ofiarę
  - Ofiara myśli, że nasz komputer to cel
  - Cel nie jest oszukiwany
  - Przechwytujemy tylko ruch od ofiary do celu
  - Ruch od celu do ofiary idzie normalnie

## 🏗️ **Architektura**

Program wykorzystuje nowoczesne wzorce projektowe:

### **Wzorzec Singleton (App)**
- Centralny punkt aplikacji
- Zarządza cyklem życia ataku
- Koordynuje wszystkie komponenty

### **Fabryka Abstrakcji (PlatformFactory)**
- Tworzy platformowo-zależne komponenty
- Umożliwia łatwe rozszerzenie na inne systemy
- Enkapsuluje różnice między platformami

### **Klasa IPAddress**
Nowoczesna klasa do obsługi adresów IPv4 w stylu SFML:

```cpp
// Tworzenie adresów IP
IPAddress ip1("192.168.1.1");
IPAddress ip2(192, 168, 1, 1);
IPAddress ip3 = IPAddress::fromString("192.168.1.1");

// Operacje na adresach
if (ip1 == ip2) { /* ... */ }
if (ip1 < ip2) { /* ... */ }
IPAddress network = ip1 & mask;
IPAddress broadcast = ip1 | ~mask;

// Sprawdzanie typów adresów
if (ip1.isPrivate()) { /* ... */ }
if (ip1.isLocalhost()) { /* ... */ }
if (ip1.isPublic()) { /* ... */ }

// Konwersje
std::string str = ip1.toString();
uint32_t num = ip1.toUint32();
std::vector<uint8_t> bytes = ip1.toBytes();

// Predefiniowane adresy
IPAddress::Any        // 0.0.0.0
IPAddress::Localhost  // 127.0.0.1
IPAddress::Broadcast  // 255.255.255.255
```

## 📁 **Struktura plików**

```
arpspoof/
├── App.hpp/cpp                    # Główna klasa aplikacji (Singleton)
├── IPAddress.hpp/cpp              # Klasa do obsługi adresów IPv4
├── ArpSpoofer.hpp/cpp             # Klasa do wykonywania ataków ARP
├── PlatformAbstraction.hpp        # Abstrakcje platformowe
├── WindowsPlatform.hpp/cpp        # Implementacja Windows
├── PlatformFactory.cpp            # Fabryka komponentów
├── NetworkHeaders.hpp             # Definicje nagłówków sieciowych
├── main.cpp                       # Główny plik wykonywalny
├── arpspoof.vcxproj               # Projekt Visual Studio
└── README.md                      # Ten plik
```

## 🚀 **Kompilacja**

### **Wymagania**
- Visual Studio 2019/2022 z C++17
- Windows 10/11
- Uprawnienia administratora

### **Kompilacja**
1. Otwórz `arpspoof.vcxproj` w Visual Studio
2. Wybierz konfigurację Release x64
3. Skompiluj projekt (Ctrl+Shift+B)

## 📖 **Użycie**

### **Podstawowa składnia**
```bash
arpspoof.exe [OPCJE] <victim-ip> [target-ip]
```

### **Opcje**
- `--help, -h` - Wyświetla pomoc
- `--list, -l` - Lista interfejsów sieciowych
- `--interface, -i` - Określa interfejs sieciowy
- `--oneway, -o` - Tryb jednokierunkowy
- `--interval, -t` - Interwał pakietów ARP (sekundy)
- `--verbose, -v` - Szczegółowe logowanie

### **Przykłady**

**Lista interfejsów:**
```bash
arpspoof.exe --list
```

**Podstawowy atak (ofiara -> brama):**
```bash
arpspoof.exe 192.168.1.10
```

**Atak z określonym celem:**
```bash
arpspoof.exe 192.168.1.10 192.168.1.1
```

**Tryb jednokierunkowy:**
```bash
arpspoof.exe --oneway 192.168.1.10
```

**Określony interfejs i interwał:**
```bash
arpspoof.exe -i "Ethernet" -t 5 192.168.1.10
```

## 🔧 **Funkcje**

### **Automatyczne wykrywanie interfejsów**
Program automatycznie wykrywa odpowiedni interfejs sieciowy na podstawie adresu IP ofiary.

### **Rozstrzyganie adresów MAC**
Automatyczne rozstrzyganie adresów MAC za pomocą pakietów ARP request.

### **Przechwytywanie ruchu**
Program przechwytuje ruch sieciowy między ofiarą a celem i przekazuje go dalej.

### **Bezpieczne zatrzymanie**
Po naciśnięciu Ctrl+C program wysyła prawidłowe pakiety ARP aby przywrócić normalne działanie sieci.

## ⚠️ **Ostrzeżenia**

- **Program wymaga uprawnień administratora!**
- Używaj tylko w kontrolowanym środowisku
- ARP spoofing może zakłócić działanie sieci
- Nie używaj do nieautoryzowanego przechwytywania ruchu

## 🛡️ **Bezpieczeństwo**

Program implementuje następujące środki bezpieczeństwa:

- Walidacja adresów IP
- Sprawdzanie uprawnień
- Bezpieczne zarządzanie pamięcią (RAII)
- Obsługa błędów i wyjątków
- Bezpieczne zatrzymanie ataku

## 🔮 **Rozszerzenia**

Program jest zaprojektowany do łatwego rozszerzania:

- **Nowe platformy:** Dodaj implementację w `PlatformAbstraction.hpp`
- **Nowe protokoły:** Rozszerz `NetworkHeaders.hpp`
- **Nowe funkcje:** Dodaj metody do klasy `App`
- **GUI:** Można dodać interfejs graficzny używając callbacków

## 📝 **Licencja**

Ten projekt jest przeznaczony wyłącznie do celów edukacyjnych i testowania bezpieczeństwa w kontrolowanym środowisku.

## 🤝 **Wkład**

Projekt jest otwarty na wkład społeczności. Proszę o:

- Zgłaszanie błędów
- Propozycje nowych funkcji
- Poprawki kodu
- Dokumentację

---

**Uwaga:** Używaj tego narzędzia odpowiedzialnie i tylko w środowiskach, które kontrolujesz lub masz uprawnienia do testowania. 