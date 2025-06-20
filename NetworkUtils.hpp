#pragma once

#include <string>
#include <vector>
#include <cstdint>

////////////////////////////////////////////////////////////
/// \brief Klasa zawierająca narzędzia do konwersji adresów sieciowych
///
/// Klasa ta zawiera statyczne metody do konwersji adresów IP i MAC
/// między różnymi formatami. Wszystkie metody są statyczne, więc
/// nie wymaga tworzenia instancji klasy.
///
/// Nazwa klasy "NetworkUtils" pochodzi od:
/// - "Network" - oznacza operacje sieciowe
/// - "Utils" - skrót od "Utilities" (narzędzia)
///
/// \see InterfaceInfo, ArpSpoofer
///
////////////////////////////////////////////////////////////
class NetworkUtils {
public:
	////////////////////////////////////////////////////////////
	/// \brief Konwertuje ciąg znaków Unicode (UTF-16) na standardowy string ASCII
	///
	/// Funkcja ta jest niezbędna w środowisku Windows, ponieważ większość API systemowych
	/// zwraca nazwy interfejsów sieciowych w formacie Unicode (wchar_t*). Aby móc je
	/// wyświetlić w konsoli lub porównać z innymi stringami, musimy je przekonwertować
	/// na standardowy format std::string.
	///
	/// \param unistr Wskaźnik na ciąg znaków Unicode (UTF-16) do przekonwertowania
	///               Może być nullptr - w takim przypadku zwracany jest pusty string
	///
	/// \return std::string Przekonwertowany ciąg znaków w formacie ASCII
	///         Jeśli konwersja się nie powiedzie lub unistr jest nullptr, zwracany jest pusty string
	///
	////////////////////////////////////////////////////////////
	static std::string unicode_to_str(wchar_t* unistr);

	////////////////////////////////////////////////////////////
	/// \brief Konwertuje adres IP z formatu bajtowego na czytelny string
	///
	/// Funkcja ta przekształca adres IP przechowywany jako tablica 4 bajtów
	/// na standardowy format kropkowy (np. "192.168.1.1"). Jest używana
	/// do wyświetlania adresów IP w interfejsie użytkownika oraz do
	/// debugowania i logowania.
	///
	/// \param ip Tablica 4 bajtów reprezentująca adres IPv4
	///           Bajty są w kolejności network byte order (big-endian)
	///
	/// \return std::string Adres IP w formacie kropkowym (np. "192.168.1.1")
	///
	////////////////////////////////////////////////////////////
	static std::string ip_to_str(const uint8_t ip[4]);

	////////////////////////////////////////////////////////////
	/// \brief Konwertuje adres MAC z formatu bajtowego na czytelny string
	///
	/// Funkcja ta przekształca adres MAC (Media Access Control) przechowywany
	/// jako tablica 6 bajtów na standardowy format z dwukropkami (np. "00:11:22:33:44:55").
	/// Jest używana do wyświetlania adresów MAC w interfejsie użytkownika oraz
	/// do debugowania i logowania operacji sieciowych.
	///
	/// \param mac Tablica 6 bajtów reprezentująca adres MAC
	///            Każdy bajt reprezentuje jedną część adresu MAC
	///
	/// \return std::string Adres MAC w formacie z dwukropkami (np. "00:11:22:33:44:55")
	///
	////////////////////////////////////////////////////////////
	static std::string mac_to_str(const uint8_t mac[6]);
}; 