# Projekt Xcode - ARP Spoofing Tool

## Przegląd

Utworzono kompletny projekt Xcode dla narzędzia ARP spoofing, który umożliwia łatwą kompilację i debugowanie na macOS.

## Struktura projektu

```
arpspoof.xcodeproj/
├── project.pbxproj              # Główny plik projektu
├── xcshareddata/
│   └── xcschemes/
│       └── arpspoof.xcscheme    # Schemat budowania i uruchamiania
└── xcuserdata/
    └── user.xcuserstate         # Stan użytkownika
```

## Funkcje projektu

### 🔧 **Ustawienia kompilacji**
- **Standard C++**: C++17
- **Platforma**: macOS 10.14+ (Mojave)
- **Kompilator**: Clang/LLVM
- **Flagi**: `-D__APPLE__` dla detekcji platformy
- **Optymalizacja**: Debug/Release konfiguracje

### 📁 **Pliki źródłowe**
- `main.cpp` - Punkt wejścia
- `App.cpp` - Główna logika aplikacji
- `ArpSpoofer.cpp` - Implementacja ARP spoofing
- `IPAddress.cpp` - Operacje na adresach IP
- `PlatformFactory.cpp` - Fabryka platform
- `MacOSPlatform.cpp` - Implementacja macOS

### 📋 **Pliki nagłówkowe**
- `App.hpp` - Interfejs aplikacji
- `ArpSpoofer.hpp` - Interfejs ARP spoofing
- `IPAddress.hpp` - Klasa adresów IP
- `PlatformAbstraction.hpp` - Abstrakcje platform
- `MacOSPlatform.hpp` - Implementacja macOS
- `NetworkHeaders.hpp` - Nagłówki sieciowe

## Uruchomienie projektu

### 1. **Otwórz projekt w Xcode**
```bash
open arpspoof.xcodeproj
```

### 2. **Wybierz schemat**
- Kliknij na nazwę schematu (obok przycisku Run)
- Wybierz "arpspoof"

### 3. **Skonfiguruj argumenty (opcjonalnie)**
- Product → Scheme → Edit Scheme
- Run → Arguments → Arguments Passed On Launch
- Dodaj argumenty: `en0 192.168.1.100 192.168.1.1`

### 4. **Kompiluj i uruchom**
- Cmd + R (Run)
- Cmd + B (Build)

## Konfiguracja debugowania

### **Argumenty domyślne**
Projekt jest skonfigurowany z przykładowymi argumentami:
- `en0` - interfejs sieciowy
- `192.168.1.100` - adres docelowy
- `192.168.1.1` - adres do spoofowania

### **Uprawnienia**
Pamiętaj, że aplikacja wymaga uprawnień root:
```bash
sudo ./arpspoof [interface] [target_ip] [spoofed_ip]
```

## Funkcje Xcode

### 🔍 **Debugowanie**
- Breakpointy w kodzie C++
- Inspekcja zmiennych
- Call stack
- Memory view

### 📊 **Analiza**
- Static analysis (Cmd + Shift + A)
- Memory leaks detection
- Performance profiling

### 🏗️ **Budowanie**
- Incremental builds
- Parallel compilation
- Error reporting

## Różnice od Makefile

| Funkcja | Makefile | Xcode |
|---------|----------|-------|
| Kompilacja | `make` | Cmd + B |
| Debugowanie | `make debug` | Cmd + R |
| Profilowanie | `make release` | Product → Profile |
| Analiza | - | Cmd + Shift + A |
| Breakpointy | - | Kliknij linię |
| Inspekcja | - | Variables view |

## Rozwiązywanie problemów

### **Błędy kompilacji**
1. Sprawdź czy Xcode Command Line Tools są zainstalowane
2. Upewnij się, że używasz macOS 10.14+
3. Sprawdź flagi kompilatora w Build Settings

### **Błędy uruchomienia**
1. Uruchom z uprawnieniami root (sudo)
2. Sprawdź czy interfejs sieciowy istnieje
3. Upewnij się, że adresy IP są poprawne

### **Problemy z BPF**
1. Sprawdź uprawnienia root
2. Upewnij się, że SIP nie blokuje dostępu
3. Sprawdź czy interfejs jest aktywny

## Integracja z Git

Projekt Xcode jest w pełni kompatybilny z systemem kontroli wersji:
- Pliki `.xcuserdata` są ignorowane przez Git
- Pliki `.xcshareddata` są commitowane
- Konfiguracja projektu jest współdzielona

## Następne kroki

1. **Otwórz projekt w Xcode**
2. **Skonfiguruj argumenty uruchomienia**
3. **Ustaw breakpointy w kluczowych miejscach**
4. **Uruchom w trybie debug**
5. **Przetestuj funkcjonalność**

Projekt Xcode zapewnia profesjonalne środowisko deweloperskie dla narzędzia ARP spoofing na macOS! 🚀 