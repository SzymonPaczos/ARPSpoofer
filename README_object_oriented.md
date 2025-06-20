# arpspoof - Wersja obiektowa

Ta wersja `arpspoof` została całkowicie przepisana w podejściu obiektowym z podziałem na klasy i pliki. Kod jest teraz bardziej modularny, łatwiejszy w utrzymaniu i rozszerzaniu.

## Struktura projektu

### 📁 **Pliki nagłówkowe (.hpp):**
- `NetworkUtils.hpp` - Narzędzia do konwersji adresów sieciowych
- `NetworkHeaders.hpp` - Struktury nagłówków sieciowych (Ethernet, IP, ARP)
- `InterfaceInfo.hpp` - Klasa informacji o interfejsie sieciowym
- `InterfaceManager.hpp` - Menedżer interfejsów sieciowych
- `RawSocket.hpp` - Klasa obsługi raw socket
- `ArpSpoofer.hpp` - Główna klasa odpowiedzialna za atak ARP spoofing

### 📁 **Pliki implementacji (.cpp):**
- `main.cpp` - Główny punkt wejścia programu
- `NetworkUtils.cpp` - Implementacja narzędzi sieciowych
- `InterfaceInfo.cpp` - Implementacja klasy interfejsu
- `InterfaceManager.cpp` - Implementacja menedżera interfejsów
- `RawSocket.cpp` - Implementacja raw socket
- `ArpSpoofer.cpp` - Implementacja głównej klasy

### 📁 **Pliki projektu:**
- `arpspoof_object_oriented.vcxproj` - Projekt Visual Studio

## Architektura obiektowa

### 🏗️ **Hierarchia klas:**

```
ArpSpoofer (główna klasa)
├── InterfaceManager (menedżer interfejsów)
│   └── InterfaceInfo[] (lista interfejsów)
├── RawSocket (obsługa sieci)
└── NetworkUtils (narzędzia statyczne)
```

### 🔧 **Klasy i ich odpowiedzialności:**

#### **ArpSpoofer** - Główna klasa
- Koordynuje cały proces ataku ARP spoofing
- Zarządza konfiguracją i parametrami ataku
- Obsługuje główną pętlę programu
- Implementuje bezpieczne zatrzymanie ataku

#### **InterfaceManager** - Menedżer interfejsów
- Wykrywa wszystkie aktywne interfejsy sieciowe
- Pobiera konfigurację interfejsów z systemu
- Udostępnia metody wyszukiwania interfejsów
- Rozstrzyga adresy IP na MAC

#### **InterfaceInfo** - Informacje o interfejsie
- Przechowuje dane o pojedynczym interfejsie
- Zawiera adresy IP, MAC, bramę domyślną
- Implementuje metody walidacji i porównywania

#### **RawSocket** - Obsługa sieci
- Enkapsuluje funkcjonalność raw socket
- Obsługuje wysyłanie i odbieranie pakietów
- Zarządza inicjalizacją i zamykaniem połączenia

#### **NetworkUtils** - Narzędzia sieciowe
- Statyczne metody konwersji adresów
- Konwersja Unicode na ASCII
- Formatowanie adresów IP i MAC

## Zalety wersji obiektowej

### ✅ **Modularność:**
- Każda klasa ma jasno określoną odpowiedzialność
- Łatwe dodawanie nowych funkcjonalności
- Możliwość testowania poszczególnych komponentów

### ✅ **Czytelność:**
- Kod jest lepiej zorganizowany
- Jasne interfejsy między klasami
- Szczegółowe komentarze SFML-style

### ✅ **Rozszerzalność:**
- Łatwe dodawanie nowych protokołów
- Możliwość rozszerzenia o nowe funkcje
- Wsparcie dla różnych platform

### ✅ **Bezpieczeństwo:**
- Lepsze zarządzanie zasobami (RAII)
- Automatyczne zamykanie połączeń
- Bezpieczne zatrzymanie ataku

## Kompilacja

### **Wymagania:**
- Visual Studio 2019/2022 (PlatformToolset v143)
- Windows SDK 10.0
- Uprawnienia administratora

### **Kroki kompilacji:**
1. Otwórz `arpspoof_object_oriented.vcxproj` w Visual Studio
2. Wybierz konfigurację (Debug/Release) i platformę (x86/x64)
3. Skompiluj projekt (Ctrl+Shift+B)

## Użycie

### **Lista interfejsów:**
```
arpspoof_object_oriented.exe --list
```

### **Podstawowe użycie:**
```
arpspoof_object_oriented.exe 192.168.1.10
```

### **Z określonym interfejsem:**
```
arpspoof_object_oriented.exe -i 1 192.168.1.10
```

### **Tylko w jedną stronę:**
```
arpspoof_object_oriented.exe --oneway 192.168.1.10
```

## Porównanie z wersją proceduralną

| Aspekt | Wersja proceduralna | Wersja obiektowa |
|--------|-------------------|------------------|
| Organizacja kodu | Jedna funkcja main() | Podział na klasy |
| Czytelność | Średnia | Wysoka |
| Modularność | Niska | Wysoka |
| Rozszerzalność | Trudna | Łatwa |
| Testowanie | Trudne | Łatwe |
| Zarządzanie zasobami | Ręczne | Automatyczne |
| Dokumentacja | Podstawowa | Szczegółowa |

## Rozszerzenia możliwe w przyszłości

### 🔮 **Potencjalne ulepszenia:**
1. **Wsparcie IPv6** - rozszerzenie o protokół IPv6
2. **GUI** - interfejs graficzny
3. **Pluginy** - system wtyczek
4. **Logowanie** - zaawansowane logowanie
5. **Konfiguracja** - pliki konfiguracyjne
6. **Wielowątkowość** - obsługa wielu ataków jednocześnie

### 🔧 **Nowe klasy możliwe do dodania:**
- `PacketLogger` - logowanie pakietów
- `ConfigurationManager` - zarządzanie konfiguracją
- `PluginManager` - system wtyczek
- `NetworkMonitor` - monitorowanie sieci
- `AttackProfiles` - profile ataków

## Bezpieczeństwo

⚠️ **UWAGA:** Ten program jest narzędziem do testów penetracyjnych. Używaj tylko w kontrolowanym środowisku i za zgodą właściciela sieci.

## Licencja

Ten kod jest oparty na oryginalnym projekcie arpspoof i podlega tym samym warunkom licencyjnym. 