#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

////////////////////////////////////////////////////////////
/// \brief Klasa reprezentująca adres IPv4
///
/// Klasa ta enkapsuluje adres IPv4 i zapewnia wygodne
/// operacje na adresach IP w stylu SFML. Zawiera przeciążone
/// operatory do łatwego porównywania, konwersji i manipulacji
/// adresami IP.
///
/// Nazwa klasy "IPAddress" pochodzi od:
/// - "IP" - oznacza protokół Internet Protocol
/// - "Address" - oznacza adres sieciowy
///
/// Klasa używa nowoczesnych funkcji C++17 i zapewnia
/// bezpieczne zarządzanie pamięcią.
///
/// \see MACAddress, NetworkUtils
///
////////////////////////////////////////////////////////////
class IPAddress {
private:
	std::vector<uint8_t> address; ///< Adres IP jako vector 4 bajtów

public:
	////////////////////////////////////////////////////////////
	/// \brief Konstruktor domyślny
	///
	/// Tworzy pusty adres IP (0.0.0.0).
	///
	////////////////////////////////////////////////////////////
	IPAddress();

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor z stringa
	///
	/// Tworzy adres IP z podanego stringa w formacie kropkowym.
	///
	/// \param ipString Adres IP w formacie "192.168.1.1"
	///
	/// \see fromString()
	///
	////////////////////////////////////////////////////////////
	explicit IPAddress(const std::string& ipString);

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor z tablicy bajtów
	///
	/// Tworzy adres IP z tablicy 4 bajtów.
	///
	/// \param bytes Tablica 4 bajtów reprezentująca adres IP
	///
	////////////////////////////////////////////////////////////
	explicit IPAddress(const uint8_t bytes[4]);

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor z vectora bajtów
	///
	/// Tworzy adres IP z vectora bajtów.
	///
	/// \param bytes Vector bajtów (musi mieć 4 elementy)
	///
	////////////////////////////////////////////////////////////
	explicit IPAddress(const std::vector<uint8_t>& bytes);

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor z pojedynczych bajtów
	///
	/// Tworzy adres IP z 4 pojedynczych bajtów.
	///
	/// \param b1 Pierwszy bajt
	/// \param b2 Drugi bajt
	/// \param b3 Trzeci bajt
	/// \param b4 Czwarty bajt
	///
	////////////////////////////////////////////////////////////
	IPAddress(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor kopiujący
	///
	/// \param other Adres IP do skopiowania
	///
	////////////////////////////////////////////////////////////
	IPAddress(const IPAddress& other) = default;

	////////////////////////////////////////////////////////////
	/// \brief Operator przypisania kopiujący
	///
	/// \param other Adres IP do przypisania
	///
	/// \return IPAddress& Referencja do tego obiektu
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator=(const IPAddress& other) = default;

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor przenoszący
	///
	/// \param other Adres IP do przeniesienia
	///
	////////////////////////////////////////////////////////////
	IPAddress(IPAddress&& other) noexcept = default;

	////////////////////////////////////////////////////////////
	/// \brief Operator przypisania przenoszący
	///
	/// \param other Adres IP do przeniesienia
	///
	/// \return IPAddress& Referencja do tego obiektu
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator=(IPAddress&& other) noexcept = default;

	////////////////////////////////////////////////////////////
	/// \brief Destruktor
	///
	/// Automatycznie zwalnia zasoby.
	///
	////////////////////////////////////////////////////////////
	~IPAddress() = default;

	////////////////////////////////////////////////////////////
	/// \brief Operator porównania równości
	///
	/// \param other Adres IP do porównania
	///
	/// \return bool true jeśli adresy są identyczne
	///
	/// \see operator!=()
	///
	////////////////////////////////////////////////////////////
	bool operator==(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator porównania nierówności
	///
	/// \param other Adres IP do porównania
	///
	/// \return bool true jeśli adresy są różne
	///
	/// \see operator==()
	///
	////////////////////////////////////////////////////////////
	bool operator!=(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator porównania mniejszości
	///
	/// Porównuje adresy IP leksykograficznie.
	///
	/// \param other Adres IP do porównania
	///
	/// \return bool true jeśli ten adres jest mniejszy
	///
	/// \see operator>(), operator<=(), operator>=()
	///
	////////////////////////////////////////////////////////////
	bool operator<(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator porównania większości
	///
	/// Porównuje adresy IP leksykograficznie.
	///
	/// \param other Adres IP do porównania
	///
	/// \return bool true jeśli ten adres jest większy
	///
	/// \see operator<(), operator<=(), operator>=()
	///
	////////////////////////////////////////////////////////////
	bool operator>(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator porównania mniejszości lub równości
	///
	/// \param other Adres IP do porównania
	///
	/// \return bool true jeśli ten adres jest mniejszy lub równy
	///
	/// \see operator<(), operator>(), operator>=()
	///
	////////////////////////////////////////////////////////////
	bool operator<=(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator porównania większości lub równości
	///
	/// \param other Adres IP do porównania
	///
	/// \return bool true jeśli ten adres jest większy lub równy
	///
	/// \see operator<(), operator>(), operator<=()
	///
	////////////////////////////////////////////////////////////
	bool operator>=(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator dostępu do bajtu
	///
	/// Umożliwia dostęp do pojedynczego bajtu adresu IP.
	///
	/// \param index Indeks bajtu (0-3)
	///
	/// \return uint8_t& Referencja do bajtu
	///
	/// \see at()
	///
	////////////////////////////////////////////////////////////
	uint8_t& operator[](size_t index);

	////////////////////////////////////////////////////////////
	/// \brief Operator dostępu do bajtu (const)
	///
	/// Umożliwia dostęp do pojedynczego bajtu adresu IP (tylko odczyt).
	///
	/// \param index Indeks bajtu (0-3)
	///
	/// \return const uint8_t& Referencja do bajtu
	///
	/// \see at()
	///
	////////////////////////////////////////////////////////////
	const uint8_t& operator[](size_t index) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator konwersji na string
	///
	/// Konwertuje adres IP na string w formacie kropkowym.
	///
	/// \return std::string Adres IP jako string
	///
	/// \see toString()
	///
	////////////////////////////////////////////////////////////
	operator std::string() const;

	////////////////////////////////////////////////////////////
	/// \brief Operator konwersji na vector bajtów
	///
	/// Konwertuje adres IP na vector bajtów.
	///
	/// \return std::vector<uint8_t> Adres IP jako vector bajtów
	///
	/// \see toBytes()
	///
	////////////////////////////////////////////////////////////
	operator std::vector<uint8_t>() const;

	////////////////////////////////////////////////////////////
	/// \brief Operator bitowego AND
	///
	/// Wykonuje operację bitowego AND na adresach IP.
	/// Użyteczne do obliczania adresów sieci.
	///
	/// \param other Adres IP do operacji AND
	///
	/// \return IPAddress Wynik operacji AND
	///
	/// \see operator|(), operator^()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator&(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator bitowego OR
	///
	/// Wykonuje operację bitowego OR na adresach IP.
	///
	/// \param other Adres IP do operacji OR
	///
	/// \return IPAddress Wynik operacji OR
	///
	/// \see operator&(), operator^()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator|(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator bitowego XOR
	///
	/// Wykonuje operację bitowego XOR na adresach IP.
	///
	/// \param other Adres IP do operacji XOR
	///
	/// \return IPAddress Wynik operacji XOR
	///
	/// \see operator&(), operator|()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator^(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator bitowego NOT
	///
	/// Wykonuje operację bitowego NOT na adresie IP.
	///
	/// \return IPAddress Wynik operacji NOT
	///
	////////////////////////////////////////////////////////////
	IPAddress operator~() const;

	////////////////////////////////////////////////////////////
	/// \brief Operator przesunięcia w lewo
	///
	/// Przesuwa bity adresu IP w lewo.
	///
	/// \param shift Liczba pozycji do przesunięcia
	///
	/// \return IPAddress Wynik przesunięcia
	///
	/// \see operator>>()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator<<(int shift) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator przesunięcia w prawo
	///
	/// Przesuwa bity adresu IP w prawo.
	///
	/// \param shift Liczba pozycji do przesunięcia
	///
	/// \return IPAddress Wynik przesunięcia
	///
	/// \see operator<<()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator>>(int shift) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator dodawania
	///
	/// Dodaje liczbę do adresu IP (modulo 2^32).
	///
	/// \param value Liczba do dodania
	///
	/// \return IPAddress Wynik dodawania
	///
	/// \see operator-()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator+(uint32_t value) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator odejmowania
	///
	/// Odejmuje liczbę od adresu IP (modulo 2^32).
	///
	/// \param value Liczba do odjęcia
	///
	/// \return IPAddress Wynik odejmowania
	///
	/// \see operator+()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator-(uint32_t value) const;

	////////////////////////////////////////////////////////////
	/// \brief Operator inkrementacji (prefix)
	///
	/// Zwiększa adres IP o 1.
	///
	/// \return IPAddress& Referencja do tego obiektu
	///
	/// \see operator++(int)
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator++();

	////////////////////////////////////////////////////////////
	/// \brief Operator inkrementacji (postfix)
	///
	/// Zwiększa adres IP o 1.
	///
	/// \return IPAddress Kopia obiektu przed inkrementacją
	///
	/// \see operator++()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator++(int);

	////////////////////////////////////////////////////////////
	/// \brief Operator dekrementacji (prefix)
	///
	/// Zmniejsza adres IP o 1.
	///
	/// \return IPAddress& Referencja do tego obiektu
	///
	/// \see operator--(int)
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator--();

	////////////////////////////////////////////////////////////
	/// \brief Operator dekrementacji (postfix)
	///
	/// Zmniejsza adres IP o 1.
	///
	/// \return IPAddress Kopia obiektu przed dekrementacją
	///
	/// \see operator--()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator--(int);

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest poprawny
	///
	/// \return bool true jeśli adres IP jest poprawny
	///
	/// \see isEmpty()
	///
	////////////////////////////////////////////////////////////
	bool isValid() const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest pusty
	///
	/// \return bool true jeśli adres IP jest pusty (0.0.0.0)
	///
	/// \see isValid()
	///
	////////////////////////////////////////////////////////////
	bool isEmpty() const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest lokalny
	///
	/// \return bool true jeśli adres IP jest lokalny (127.x.x.x)
	///
	/// \see isPrivate(), isPublic()
	///
	////////////////////////////////////////////////////////////
	bool isLocalhost() const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest prywatny
	///
	/// \return bool true jeśli adres IP jest prywatny
	///
	/// \see isLocalhost(), isPublic()
	///
	////////////////////////////////////////////////////////////
	bool isPrivate() const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest publiczny
	///
	/// \return bool true jeśli adres IP jest publiczny
	///
	/// \see isLocalhost(), isPrivate()
	///
	////////////////////////////////////////////////////////////
	bool isPublic() const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest w tej samej sieci
	///
	/// \param other Adres IP do sprawdzenia
	/// \param mask Maska sieci
	///
	/// \return bool true jeśli adresy są w tej samej sieci
	///
	////////////////////////////////////////////////////////////
	bool isInSameNetwork(const IPAddress& other, const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Konwertuje adres IP na string
	///
	/// \return std::string Adres IP w formacie kropkowym
	///
	/// \see fromString()
	///
	////////////////////////////////////////////////////////////
	std::string toString() const;

	////////////////////////////////////////////////////////////
	/// \brief Konwertuje adres IP na vector bajtów
	///
	/// \return std::vector<uint8_t> Adres IP jako vector bajtów
	///
	/// \see fromBytes()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> toBytes() const;

	////////////////////////////////////////////////////////////
	/// \brief Konwertuje adres IP na uint32_t
	///
	/// \return uint32_t Adres IP jako liczba 32-bitowa
	///
	/// \see fromUint32()
	///
	////////////////////////////////////////////////////////////
	uint32_t toUint32() const;

	////////////////////////////////////////////////////////////
	/// \brief Tworzy adres IP ze stringa
	///
	/// \param ipString Adres IP w formacie kropkowym
	///
	/// \return IPAddress Adres IP lub pusty adres jeśli błąd
	///
	/// \see toString()
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromString(const std::string& ipString);

	////////////////////////////////////////////////////////////
	/// \brief Tworzy adres IP z vectora bajtów
	///
	/// \param bytes Vector bajtów (musi mieć 4 elementy)
	///
	/// \return IPAddress Adres IP lub pusty adres jeśli błąd
	///
	/// \see toBytes()
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromBytes(const std::vector<uint8_t>& bytes);

	////////////////////////////////////////////////////////////
	/// \brief Tworzy adres IP z uint32_t
	///
	/// \param value Liczba 32-bitowa reprezentująca adres IP
	///
	/// \return IPAddress Adres IP
	///
	/// \see toUint32()
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromUint32(uint32_t value);

	////////////////////////////////////////////////////////////
	/// \brief Tworzy maskę sieci z długości prefiksu
	///
	/// \param prefixLength Długość prefiksu (0-32)
	///
	/// \return IPAddress Maska sieci
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromPrefixLength(uint8_t prefixLength);

	////////////////////////////////////////////////////////////
	/// \brief Pobiera adres sieci
	///
	/// \param mask Maska sieci
	///
	/// \return IPAddress Adres sieci
	///
	////////////////////////////////////////////////////////////
	IPAddress getNetworkAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera adres broadcast
	///
	/// \param mask Maska sieci
	///
	/// \return IPAddress Adres broadcast
	///
	////////////////////////////////////////////////////////////
	IPAddress getBroadcastAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera pierwszy adres hosta
	///
	/// \param mask Maska sieci
	///
	/// \return IPAddress Pierwszy adres hosta
	///
	////////////////////////////////////////////////////////////
	IPAddress getFirstHostAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera ostatni adres hosta
	///
	/// \param mask Maska sieci
	///
	/// \return IPAddress Ostatni adres hosta
	///
	////////////////////////////////////////////////////////////
	IPAddress getLastHostAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera liczbę hostów w sieci
	///
	/// \param mask Maska sieci
	///
	/// \return uint32_t Liczba hostów w sieci
	///
	////////////////////////////////////////////////////////////
	uint32_t getHostCount(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Bezpieczny dostęp do bajtu
	///
	/// \param index Indeks bajtu (0-3)
	///
	/// \return uint8_t Bajt adresu IP
	///
	/// \throws std::out_of_range jeśli indeks jest nieprawidłowy
	///
	/// \see operator[]()
	///
	////////////////////////////////////////////////////////////
	uint8_t at(size_t index) const;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera rozmiar adresu IP
	///
	/// \return size_t Rozmiar adresu IP (zawsze 4)
	///
	////////////////////////////////////////////////////////////
	size_t size() const { return 4; }

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy adres IP jest pusty
	///
	/// \return bool true jeśli adres IP jest pusty
	///
	////////////////////////////////////////////////////////////
	bool empty() const { return address.empty(); }

	////////////////////////////////////////////////////////////
	/// \brief Pobiera wskaźnik do danych
	///
	/// \return const uint8_t* Wskaźnik do danych adresu IP
	///
	////////////////////////////////////////////////////////////
	const uint8_t* data() const { return address.data(); }

	////////////////////////////////////////////////////////////
	/// \brief Pobiera iterator na początek
	///
	/// \return std::vector<uint8_t>::const_iterator Iterator na początek
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t>::const_iterator begin() const { return address.begin(); }

	////////////////////////////////////////////////////////////
	/// \brief Pobiera iterator na koniec
	///
	/// \return std::vector<uint8_t>::const_iterator Iterator na koniec
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t>::const_iterator end() const { return address.end(); }

	// Predefiniowane adresy IP
	static const IPAddress Any;        ///< 0.0.0.0 - dowolny adres
	static const IPAddress Localhost;  ///< 127.0.0.1 - localhost
	static const IPAddress Broadcast;  ///< 255.255.255.255 - broadcast
};

////////////////////////////////////////////////////////////
/// \brief Operator wyjścia dla IPAddress
///
/// Wyświetla adres IP w formacie kropkowym.
///
/// \param os Strumień wyjścia
/// \param ip Adres IP do wyświetlenia
///
/// \return std::ostream& Referencja do strumienia
///
/// \see operator>>()
///
////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const IPAddress& ip);

////////////////////////////////////////////////////////////
/// \brief Operator wejścia dla IPAddress
///
/// Wczytuje adres IP ze strumienia w formacie kropkowym.
///
/// \param is Strumień wejścia
/// \param ip Adres IP do wczytania
///
/// \return std::istream& Referencja do strumienia
///
/// \see operator<<()
///
////////////////////////////////////////////////////////////
std::istream& operator>>(std::istream& is, IPAddress& ip); 