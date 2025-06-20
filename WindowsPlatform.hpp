#pragma once

#include "PlatformAbstraction.hpp"

#ifdef _WIN32

////////////////////////////////////////////////////////////
/// \brief Implementacja NetworkInterface dla Windows
///
/// Klasa ta implementuje interfejs NetworkInterface
/// używając Windows API (GetAdaptersAddresses, ResolveIpNetEntry2).
/// Zapewnia dostęp do informacji o interfejsach sieciowych
/// oraz rozstrzyganie adresów MAC na platformie Windows.
///
/// Nazwa klasy "WindowsNetworkInterface" pochodzi od:
/// - "Windows" - oznacza platformę Windows
/// - "Network" - oznacza operacje sieciowe
/// - "Interface" - oznacza implementację interfejsu
///
/// \see NetworkInterface, PlatformFactory
///
////////////////////////////////////////////////////////////
class WindowsNetworkInterface : public NetworkInterface {
public:
	////////////////////////////////////////////////////////////
	/// \brief Konstruktor domyślny
	///
	/// Inicjalizuje obiekt WindowsNetworkInterface.
	///
	////////////////////////////////////////////////////////////
	WindowsNetworkInterface() = default;

	////////////////////////////////////////////////////////////
	/// \brief Destruktor
	///
	/// Zwalnia zasoby związane z Windows API.
	///
	////////////////////////////////////////////////////////////
	~WindowsNetworkInterface() override = default;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera listę wszystkich interfejsów sieciowych
	///
	/// Implementacja dla Windows używająca GetAdaptersAddresses()
	/// do pobrania informacji o wszystkich adapterach sieciowych.
	///
	/// \return std::vector<InterfaceInfo> Lista interfejsów sieciowych
	///
	/// \see NetworkInterface::getInterfaces()
	///
	////////////////////////////////////////////////////////////
	std::vector<InterfaceInfo> getInterfaces() override;

	////////////////////////////////////////////////////////////
	/// \brief Rozwiązuje adres IP na adres MAC
	///
	/// Implementacja dla Windows używająca ResolveIpNetEntry2()
	/// do wyszukania adresu MAC w tablicy ARP systemu.
	///
	/// \param interfaceName Nazwa interfejsu na którym szukać
	/// \param ip Adres IP do rozstrzygnięcia
	///
	/// \return std::vector<uint8_t> Adres MAC (6 bajtów) lub pusty vector
	///
	/// \see NetworkInterface::resolveMacAddress()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> resolveMacAddress(const std::string& interfaceName, 
	                                       const std::vector<uint8_t>& ip) override;

private:
	////////////////////////////////////////////////////////////
	/// \brief Konwertuje ciąg Unicode na string
	///
	/// Funkcja pomocnicza do konwersji nazw interfejsów
	/// z formatu Unicode na standardowy string.
	///
	/// \param unistr Wskaźnik na ciąg Unicode
	///
	/// \return std::string Przekonwertowany string
	///
	////////////////////////////////////////////////////////////
	static std::string unicodeToString(const wchar_t* unistr);
};

////////////////////////////////////////////////////////////
/// \brief Implementacja RawSocket dla Windows
///
/// Klasa ta implementuje interfejs RawSocket używając
/// Windows API (Winsock2). Zapewnia możliwość wysyłania
/// i odbierania pakietów sieciowych na poziomie warstwy
/// sieciowej na platformie Windows.
///
/// Nazwa klasy "WindowsRawSocket" pochodzi od:
/// - "Windows" - oznacza platformę Windows
/// - "Raw" - oznacza surowe pakiety sieciowe
/// - "Socket" - oznacza gniazdo sieciowe
///
/// \see RawSocket, PlatformFactory
///
////////////////////////////////////////////////////////////
class WindowsRawSocket : public RawSocket {
public:
	////////////////////////////////////////////////////////////
	/// \brief Konstruktor domyślny
	///
	/// Inicjalizuje obiekt WindowsRawSocket z domyślnymi wartościami.
	///
	////////////////////////////////////////////////////////////
	WindowsRawSocket();

	////////////////////////////////////////////////////////////
	/// \brief Destruktor
	///
	/// Automatycznie zamyka socket i zwalnia zasoby Winsock.
	///
	/// \see close()
	///
	////////////////////////////////////////////////////////////
	~WindowsRawSocket() override;

	////////////////////////////////////////////////////////////
	/// \brief Otwiera raw socket
	///
	/// Implementacja dla Windows używająca socket() i setsockopt()
	/// do utworzenia raw socket z opcjonalnym trybem promiscuous.
	///
	/// \param interfaceName Nazwa interfejsu sieciowego
	/// \param promiscuous Czy włączyć tryb promiscuous
	///
	/// \return bool true jeśli socket został pomyślnie otwarty
	///
	/// \see RawSocket::open()
	///
	////////////////////////////////////////////////////////////
	bool open(const std::string& interfaceName, bool promiscuous = true) override;

	////////////////////////////////////////////////////////////
	/// \brief Zamyka raw socket
	///
	/// Implementacja dla Windows używająca closesocket()
	/// i WSACleanup() do bezpiecznego zamknięcia połączenia.
	///
	/// \see RawSocket::close()
	///
	////////////////////////////////////////////////////////////
	void close() override;

	////////////////////////////////////////////////////////////
	/// \brief Wysyła pakiet przez raw socket
	///
	/// Implementacja dla Windows używająca sendto()
	/// do wysłania danych przez raw socket.
	///
	/// \param data Dane do wysłania
	///
	/// \return bool true jeśli pakiet został pomyślnie wysłany
	///
	/// \see RawSocket::sendPacket()
	///
	////////////////////////////////////////////////////////////
	bool sendPacket(const std::vector<uint8_t>& data) override;

	////////////////////////////////////////////////////////////
	/// \brief Odbiera pakiet z raw socket
	///
	/// Implementacja dla Windows używająca recv()
	/// do odbioru danych z raw socket.
	///
	/// \return std::vector<uint8_t> Odebrane dane lub pusty vector
	///
	/// \see RawSocket::receivePacket()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> receivePacket() override;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy socket jest otwarty
	///
	/// \return bool true jeśli socket jest otwarty
	///
	/// \see RawSocket::isOpen()
	///
	////////////////////////////////////////////////////////////
	bool isOpen() const override;

private:
	void* sock;        ///< Deskryptor gniazda Windows (SOCKET)
	bool initialized;   ///< Czy Winsock został zainicjalizowany
};

#endif // _WIN32 