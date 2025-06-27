# Tryb Porzucania Pakietów (Drop Mode)

## 🚫 **Opis funkcjonalności**

Tryb porzucania pakietów (`--drop` lub `-d`) to nowa opcja w programie ARP Spoofing Tool, która pozwala na **odcięcie internetu** między dwoma urządzeniami sieciowymi poprzez porzucanie pakietów zamiast ich przekazywania.

## ⚠️ **Ostrzeżenie**

**UWAGA: Ta funkcjonalność odcina internet między urządzeniami!**
- Używaj tylko w kontrolowanym środowisku
- Upewnij się, że masz uprawnienia do testowania
- Może zakłócić normalną pracę sieci

## 🔧 **Jak to działa**

### **Tryb normalny (domyślny):**
1. Program przechwytuje pakiety między urządzeniami
2. Modyfikuje nagłówki Ethernet
3. **Przekazuje pakiety dalej** do właściwych odbiorców
4. Internet działa normalnie, ale ruch przechodzi przez atakującego

### **Tryb porzucania (--drop):**
1. Program przechwytuje pakiety między urządzeniami
2. **Porzuca pakiety** zamiast je przekazywać
3. Urządzenia nie mogą się komunikować
4. **Internet zostaje odcięty**

## 📋 **Sposoby użycia**

### **Z wiersza poleceń:**
```bash
# Odcina internet między 192.168.1.10 a bramą domyślną
sudo ./arpspoof --drop 192.168.1.10

# Odcina internet między dwoma konkretnymi urządzeniami
sudo ./arpspoof --drop 192.168.1.10 192.168.1.1

# Tryb jednokierunkowy + porzucanie (tylko od ofiary do celu)
sudo ./arpspoof --drop --oneway 192.168.1.10
```

### **W trybie interaktywnym:**
```bash
./arpspoof
# Program zapyta: "Czy chcesz odrzucić pakiety? (t/n, domyślnie n): "
# Po wybraniu "t" pojawi się ostrzeżenie i potwierdzenie
```

## 📊 **Statystyki**

W trybie porzucania program wyświetla dodatkowe statystyki:

```
Statystyki: Wysłano 15 ARP, Odebrano 127, Porzucono 127 pakietów
```

### **Końcowe statystyki:**
```
Atak zakończony. Statystyki końcowe:
  - Wysłano pakietów ARP: 45
  - Odebrano pakietów: 342
  - Porzucono pakietów: 342
  - Internet został odcięty na 342 pakietów
```

## 🎯 **Przypadki użycia**

### **Testowanie zabezpieczeń:**
- Sprawdzenie czy system wykrywa ataki ARP
- Testowanie mechanizmów obrony przed ARP spoofing
- Weryfikacja logów bezpieczeństwa

### **Edukacja:**
- Demonstracja skutków ataków ARP
- Pokazanie jak działa man-in-the-middle
- Nauka o bezpieczeństwie sieci

### **Kontrola dostępu:**
- Tymczasowe odcięcie urządzenia od sieci
- Izolacja podejrzanego ruchu
- Testowanie redundancji sieci

## 🔒 **Bezpieczeństwo**

### **Ostrzeżenia:**
- Używaj tylko w środowisku testowym
- Upewnij się, że masz uprawnienia
- Nie używaj na produkcyjnych sieciach
- Może zakłócić pracę innych użytkowników

### **Ograniczenia:**
- Działa tylko na IPv4
- Wymaga uprawnień administratora
- Może być wykryte przez systemy IDS/IPS
- Nie działa na wszystkich typach sieci

## 🛠️ **Implementacja techniczna**

### **Zmiany w kodzie:**
1. **App.hpp**: Dodano `bool dropMode` do `AttackConfig`
2. **App.cpp**: Modyfikacja `handlePacket()` - porzucanie zamiast przekazywania
3. **main.cpp**: Obsługa opcji `--drop` w parserze argumentów
4. **Statystyki**: Dodano licznik porzuconych pakietów

### **Kluczowy kod:**
```cpp
// W trybie dropMode porzuć pakiet zamiast go przekazywać
if (config.dropMode) {
    attackInfo.packetsDropped++;
    return; // Porzuć pakiet - nie przekazuj dalej
}
```

## 📝 **Przykłady użycia**

### **Test 1: Odciecie pojedynczego urządzenia**
```bash
sudo ./arpspoof --drop 192.168.1.100
# Urządzenie 192.168.1.100 straci połączenie z internetem
```

### **Test 2: Odciecie komunikacji między urządzeniami**
```bash
sudo ./arpspoof --drop 192.168.1.10 192.168.1.20
# Urządzenia nie mogą się komunikować ze sobą
```

### **Test 3: Tryb jednokierunkowy**
```bash
sudo ./arpspoof --drop --oneway 192.168.1.10
# Tylko ruch od 192.168.1.10 do bramy jest blokowany
```

## ⚡ **Wydajność**

- **Minimalny wpływ na CPU**: Program tylko porzuca pakiety
- **Szybkie działanie**: Brak przetwarzania pakietów
- **Niskie zużycie pamięci**: Nie buforuje pakietów
- **Natychmiastowy efekt**: Internet odcięty od pierwszego pakietu

## 🔄 **Przywracanie połączenia**

Po zatrzymaniu ataku (Ctrl+C) program automatycznie:
1. Wysyła prawidłowe pakiety ARP
2. Przywraca normalną komunikację
3. Wyświetla statystyki końcowe

```bash
# Naciśnij Ctrl+C aby zatrzymać
^C
Atak zakończony. Statystyki końcowe:
  - Wysłano pakietów ARP: 23
  - Odebrano pakietów: 156
  - Porzucono pakietów: 156
  - Internet został odcięty na 156 pakietów
Atak zatrzymany
``` 