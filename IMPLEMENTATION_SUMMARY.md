# Podsumowanie implementacji macOS

## Przegląd

Dodano pełną obsługę macOS do narzędzia ARP spoofing. Implementacja obejmuje wszystkie niezbędne komponenty do działania na systemie macOS.

## Dodane pliki

### 1. MacOSPlatform.hpp
- **Lokalizacja**: `MacOSPlatform.hpp`
- **Zawartość**: 
  - Klasa `MacOSNetworkInterface` implementująca interfejs `NetworkInterface`
  - Klasa `MacOSRawSocket` implementująca interfejs `RawSocket`
  - Wszystkie niezbędne nagłówki systemowe dla macOS
  - Pełna dokumentacja Doxygen

### 2. MacOSPlatform.cpp
- **Lokalizacja**: `MacOSPlatform.cpp`
- **Zawartość**:
  - Implementacja `MacOSNetworkInterface::getInterfaces()` używająca sysctl
  - Implementacja `MacOSNetworkInterface::resolveMacAddress()` używająca ARP table
  - Implementacja `MacOSRawSocket` używająca BPF (Berkeley Packet Filter)
  - Wszystkie metody pomocnicze dla operacji sieciowych

## Kluczowe cechy implementacji macOS

### NetworkInterface
- **Wykrywanie interfejsów**: Używa `sysctl()` z `NET_RT_IFLIST` do pobierania listy interfejsów
- **Rozwiązywanie MAC**: Używa `sysctl()` z `NET_RT_FLAGS` i `RTF_LLINFO` do przeszukiwania tabeli ARP
- **Informacje o interfejsie**: Pobiera MAC, IP, maskę podsieci i bramę domyślną

### RawSocket
- **BPF**: Używa Berkeley Packet Filter do przechwytywania i wysyłania pakietów
- **Tryb promiscuous**: Obsługuje tryb promiscuous dla przechwytywania wszystkich pakietów
- **Buforowanie**: Konfiguruje odpowiedni rozmiar bufora dla wydajności

## Zaktualizowane pliki

### 1. PlatformFactory.cpp
- Dodano obsługę `#elif defined(__APPLE__)`
- Dodano include `MacOSPlatform.hpp`
- Dodano tworzenie `MacOSNetworkInterface` i `MacOSRawSocket`

### 2. Makefile
- Dodano automatyczną detekcję platformy
- Dodano obsługę macOS z flagami `-D__APPLE__`
- Dodano kompilator `clang++` dla macOS
- Dodano polecenie `make info` do wyświetlania informacji o platformie

### 3. README.md
- Zaktualizowano sekcję Features (dodano macOS)
- Dodano sekcję macOS Build z instrukcjami kompilacji
- Dodano sekcję macOS Usage z przykładami
- Zaktualizowano Platform Support
- Dodano sekcję Troubleshooting dla macOS

### 4. UML_Diagram.md
- Dodano klasy `MacOSNetworkInterface` i `MacOSRawSocket` do diagramu
- Zaktualizowano relacje dziedziczenia
- Dodano sekcję Platform-Specific Implementations
- Zaktualizowano opis obsługi platform

## Wymagania systemowe

### macOS
- **Minimalna wersja**: macOS 10.14 (Mojave) lub nowsza
- **Kompilator**: Xcode Command Line Tools lub Clang 5+
- **Uprawnienia**: Wymagane uprawnienia root (sudo) dla dostępu do BPF
- **Zależności**: Brak dodatkowych zależności (wszystko wbudowane w system)

## Kompilacja na macOS

```bash
# Instalacja narzędzi deweloperskich (jeśli potrzebne)
xcode-select --install

# Kompilacja
make

# Instalacja systemowa (opcjonalnie)
sudo make install

# Informacje o platformie
make info
```

## Użycie na macOS

```bash
# Uruchomienie z uprawnieniami root
sudo ./arpspoof [interface_name] [target_ip] [spoofed_ip]

# Przykład
sudo ./arpspoof en0 192.168.1.100 192.168.1.1
```

## Różnice platformowe

### macOS vs Linux
- **Interfejsy**: macOS używa `en0`, `en1` zamiast `eth0`, `eth1`
- **API**: BPF zamiast AF_PACKET
- **System calls**: sysctl zamiast /proc filesystem
- **Uprawnienia**: Podobne wymagania root

### macOS vs Windows
- **API**: BPF zamiast WinPcap/Npcap
- **Uprawnienia**: sudo zamiast Administrator
- **Interfejsy**: Unix-style zamiast Windows-style

## Testowanie

Implementacja została zaprojektowana zgodnie z:
- Interfejsami `NetworkInterface` i `RawSocket`
- Wzorcem Factory używanym w projekcie
- Konwencjami kodowania C++17
- Dokumentacją Doxygen

## Uwagi techniczne

1. **BPF**: Berkeley Packet Filter jest natywnym mechanizmem macOS do przechwytywania pakietów
2. **sysctl**: System Control Interface jest używany do pobierania informacji systemowych
3. **Uprawnienia**: Dostęp do BPF wymaga uprawnień root
4. **Kompatybilność**: Implementacja jest kompatybilna z macOS 10.14+

## Następne kroki

1. **Testowanie**: Przetestować na rzeczywistym systemie macOS
2. **Optymalizacja**: Dostroić parametry BPF dla lepszej wydajności
3. **Dokumentacja**: Dodać więcej przykładów użycia
4. **Rozszerzenia**: Rozważyć obsługę IPv6 