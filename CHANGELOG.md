# Changelog

## [1.1.0] - 2024-12-19

### Dodano
- **Tryb porzucania pakietów (Drop Mode)**
  - Nowa opcja `--drop` lub `-d` w wierszu poleceń
  - Opcja w trybie interaktywnym z ostrzeżeniem
  - Porzucanie pakietów zamiast przekazywania ich dalej
  - **Efekt: odcięcie internetu między urządzeniami**

### Zmiany w kodzie
- **App.hpp**: Dodano `bool dropMode` do struktury `AttackConfig`
- **App.hpp**: Dodano `uint64_t packetsDropped` do struktury `AttackInfo`
- **App.cpp**: Modyfikacja `handlePacket()` - logika porzucania pakietów
- **App.cpp**: Aktualizacja `startAttack()` - wyświetlanie informacji o trybie
- **App.cpp**: Dodano statystyki porzuconych pakietów
- **main.cpp**: Obsługa opcji `--drop` w parserze argumentów
- **main.cpp**: Dodano pytanie o tryb porzucania w trybie interaktywnym
- **main.cpp**: Dodano ostrzeżenie i potwierdzenie w trybie interaktywnym

### Nowe funkcje
- **Statystyki porzuconych pakietów**: Licznik i wyświetlanie co 10 sekund
- **Końcowe statystyki**: Podsumowanie po zatrzymaniu ataku
- **Ostrzeżenia**: Wyraźne komunikaty o skutkach trybu drop
- **Dokumentacja**: Szczegółowy opis w `DROP_MODE_README.md`

### Przykłady użycia
```bash
# Odcina internet między urządzeniem a bramą
sudo ./arpspoof --drop 192.168.1.100

# Odcina internet między dwoma urządzeniami
sudo ./arpspoof --drop 192.168.1.100 192.168.1.1

# Tryb jednokierunkowy + porzucanie
sudo ./arpspoof --drop --oneway 192.168.1.100
```

### Dokumentacja
- Dodano sekcję "Drop Mode" w głównym README.md
- Utworzono szczegółowy `DROP_MODE_README.md`
- Zaktualizowano pomoce i przykłady

### Bezpieczeństwo
- Dodano ostrzeżenia o skutkach trybu drop
- Podwójne potwierdzenie w trybie interaktywnym
- Wyraźne komunikaty o odcięciu internetu

## [1.0.0] - 2024-12-18

### Pierwsza wersja
- Podstawowa funkcjonalność ARP spoofing
- Obsługa Windows, Linux, macOS
- Tryb interaktywny
- Przekazywanie pakietów (man-in-the-middle)
- Statystyki i logowanie 