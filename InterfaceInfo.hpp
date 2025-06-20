#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>

////////////////////////////////////////////////////////////
/// \brief Klasa reprezentująca informacje o interfejsie sieciowym
///
/// Klasa ta zawiera wszystkie niezbędne informacje o interfejsie sieciowym
/// potrzebne do przeprowadzenia ataku ARP spoofing. Zawiera zarówno identyfikatory
/// systemowe jak i adresy sieciowe oraz konfigurację routingu.
///
/// Nazwa klasy "InterfaceInfo" pochodzi od:
/// - "Interface" - oznacza interfejs sieciowy
/// - "Info" - oznacza informacje o interfejsie
///
/// \see InterfaceManager, ArpSpoofer
///
////////////////////////////////////////////////////////////
class InterfaceInfo {
public:
	ULONG ifIndex;           ///< Indeks interfejsu w systemie Windows
	std::string name;        ///< Nazwa interfejsu w formacie systemowym
	std::string description; ///< Opis interfejsu dla użytkownika
	uint8_t mac[6];          ///< Adres MAC interfejsu
	uint8_t ip[4];           ///< Adres IP interfejsu
	uint8_t prefixlen;       ///< Długość prefiksu sieci (maska podsieci)
	uint8_t gateway[4];      ///< Adres bramy domyślnej

	////////////////////////////////////////////////////////////
	/// \brief Konstruktor domyślny
	///
	/// Inicjalizuje obiekt InterfaceInfo z domyślnymi wartościami.
	/// Wszystkie pola są inicjalizowane zerami lub pustymi stringami.
	///
	////////////////////////////////////////////////////////////
	InterfaceInfo();

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy interfejs ma poprawną konfigurację
	///
	/// Funkcja ta sprawdza czy interfejs ma ustawione wszystkie
	/// niezbędne parametry sieciowe (adres IP, MAC, brama).
	///
	/// \return bool true jeśli interfejs ma poprawną konfigurację,
	///         false jeśli brakuje ważnych parametrów
	///
	////////////////////////////////////////////////////////////
	bool isValid() const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy podany adres IP należy do tej samej sieci
	///
	/// Funkcja ta sprawdza czy podany adres IP znajduje się w tej samej
	/// podsieci co interfejs, używając maski podsieci.
	///
	/// \param ip Adres IP do sprawdzenia
	///
	/// \return bool true jeśli adres IP należy do tej samej sieci,
	///         false w przeciwnym przypadku
	///
	////////////////////////////////////////////////////////////
	bool isInSameNetwork(const uint8_t ip[4]) const;
}; 