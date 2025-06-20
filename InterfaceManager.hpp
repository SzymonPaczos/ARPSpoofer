#pragma once

#include "InterfaceInfo.hpp"
#include "NetworkUtils.hpp"
#include <vector>
#include <string>

////////////////////////////////////////////////////////////
/// \brief Klasa zarządzająca wykrywaniem i zarządzaniem interfejsami sieciowymi
///
/// Klasa ta odpowiada za wykrywanie wszystkich aktywnych interfejsów sieciowych
/// w systemie, pobieranie ich konfiguracji oraz udostępnianie metod do
/// wyszukiwania odpowiednich interfejsów na podstawie różnych kryteriów.
///
/// Nazwa klasy "InterfaceManager" pochodzi od:
/// - "Interface" - oznacza interfejsy sieciowe
/// - "Manager" - oznacza zarządzanie/wykrywanie interfejsów
///
/// \see InterfaceInfo, ArpSpoofer
///
////////////////////////////////////////////////////////////
class InterfaceManager {
private:
	std::vector<InterfaceInfo> interfaces; ///< Lista wykrytych interfejsów

public:
	////////////////////////////////////////////////////////////
	/// \brief Konstruktor domyślny
	///
	/// Inicjalizuje obiekt InterfaceManager i automatycznie
	/// wykrywa wszystkie aktywne interfejsy sieciowe w systemie.
	///
	////////////////////////////////////////////////////////////
	InterfaceManager();

	////////////////////////////////////////////////////////////
	/// \brief Wykrywa i pobiera informacje o wszystkich aktywnych interfejsach sieciowych
	///
	/// Funkcja ta skanuje system w poszukiwaniu wszystkich aktywnych interfejsów sieciowych
	/// i pobiera ich szczegółowe informacje konfiguracyjne. Jest to kluczowa funkcja
	/// która umożliwia programowi automatyczne wykrycie odpowiedniego interfejsu
	/// do przeprowadzenia ataku ARP spoofing.
	///
	/// Funkcja używa Windows API GetAdaptersAddresses() do pobrania informacji
	/// o wszystkich adapterach sieciowych w systemie.
	///
	/// \return bool true jeśli interfejsy zostały pomyślnie wykryte,
	///         false w przypadku błędu
	///
	/// \see printInterfaces(), findInterfaceByIndex(), findInterfaceByName()
	///
	////////////////////////////////////////////////////////////
	bool discoverInterfaces();

	////////////////////////////////////////////////////////////
	/// \brief Wyświetla listę wszystkich wykrytych interfejsów sieciowych
	///
	/// Funkcja ta formatuje i wyświetla w konsoli informacje o wszystkich
	/// interfejsach sieciowych wykrytych przez discoverInterfaces().
	/// Jest używana gdy użytkownik uruchamia program z opcją --list.
	///
	/// Wyświetlane informacje obejmują: numer porządkowy, nazwę systemową,
	/// opis użytkownika, adres IP z maską podsieci oraz adres bramy domyślnej.
	///
	/// \see discoverInterfaces(), getInterfaceCount()
	///
	////////////////////////////////////////////////////////////
	void printInterfaces() const;

	////////////////////////////////////////////////////////////
	/// \brief Znajduje interfejs na podstawie indeksu
	///
	/// Funkcja ta wyszukuje interfejs o podanym indeksie w liście
	/// wykrytych interfejsów.
	///
	/// \param index Indeks interfejsu do znalezienia
	///
	/// \return const InterfaceInfo* Wskaźnik do znalezionego interfejsu lub nullptr
	///
	/// \see findInterfaceByName(), findInterfaceByNetwork()
	///
	////////////////////////////////////////////////////////////
	const InterfaceInfo* findInterfaceByIndex(int index) const;

	////////////////////////////////////////////////////////////
	/// \brief Znajduje interfejs na podstawie nazwy
	///
	/// Funkcja ta wyszukuje interfejs o podanej nazwie w liście
	/// wykrytych interfejsów.
	///
	/// \param name Nazwa interfejsu do znalezienia
	///
	/// \return const InterfaceInfo* Wskaźnik do znalezionego interfejsu lub nullptr
	///
	/// \see findInterfaceByIndex(), findInterfaceByNetwork()
	///
	////////////////////////////////////////////////////////////
	const InterfaceInfo* findInterfaceByName(const std::string& name) const;

	////////////////////////////////////////////////////////////
	/// \brief Znajduje interfejs na podstawie adresu IP w sieci
	///
	/// Funkcja ta wyszukuje interfejs który ma adres IP w tej samej
	/// podsieci co podany adres IP.
	///
	/// \param ip Adres IP do sprawdzenia
	///
	/// \return const InterfaceInfo* Wskaźnik do znalezionego interfejsu lub nullptr
	///
	/// \see findInterfaceByIndex(), findInterfaceByName()
	///
	////////////////////////////////////////////////////////////
	const InterfaceInfo* findInterfaceByNetwork(const uint8_t ip[4]) const;

	////////////////////////////////////////////////////////////
	/// \brief Rozwiązuje adres IP na adres MAC używając tablicy ARP systemu
	///
	/// Funkcja ta wykorzystuje Windows API ResolveIpNetEntry2() do wyszukania
	/// adresu MAC odpowiadającego podanemu adresowi IP w tablicy ARP systemu.
	/// Jest to kluczowa funkcja która umożliwia programowi znalezienie adresów
	/// MAC ofiary i celu bez konieczności wysyłania pakietów ARP request.
	///
	/// Funkcja sprawdza czy wpis w tablicy ARP ma status "reachable",
	/// co oznacza że host jest aktywny i dostępny w sieci.
	///
	/// \param iface Referencja do interfejsu na którym ma być przeprowadzone rozwiązywanie
	/// \param ip Tablica 4 bajtów reprezentująca adres IP do rozstrzygnięcia
	/// \param mac Tablica 6 bajtów do której zostanie zapisany znaleziony adres MAC
	///
	/// \return bool true jeśli adres MAC został pomyślnie rozstrzygnięty,
	///         false jeśli nie można znaleźć wpisu lub host nie jest dostępny
	///
	/// \see discoverInterfaces(), InterfaceInfo
	///
	////////////////////////////////////////////////////////////
	bool resolveMacAddress(const InterfaceInfo& iface, const uint8_t ip[4], uint8_t mac[6]) const;

	////////////////////////////////////////////////////////////
	/// \brief Zwraca liczbę wykrytych interfejsów
	///
	/// \return size_t Liczba interfejsów w liście
	///
	/// \see discoverInterfaces()
	///
	////////////////////////////////////////////////////////////
	size_t getInterfaceCount() const { return interfaces.size(); }

	////////////////////////////////////////////////////////////
	/// \brief Zwraca interfejs o podanym indeksie
	///
	/// \param index Indeks interfejsu (0-based)
	///
	/// \return const InterfaceInfo& Referencja do interfejsu
	///
	/// \see getInterfaceCount()
	///
	////////////////////////////////////////////////////////////
	const InterfaceInfo& getInterface(size_t index) const { return interfaces[index]; }
}; 