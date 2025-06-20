#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

////////////////////////////////////////////////////////////
/// \brief Abstrakcja dla operacji sieciowych na różnych platformach
///
/// Klasa ta definiuje interfejs dla operacji sieciowych,
/// który może być implementowany na różnych platformach
/// (Windows, Linux, macOS). Umożliwia to pisanie kodu
/// wieloplatformowego bez bezpośredniego używania API systemowych.
///
/// Nazwa klasy "NetworkInterface" pochodzi od:
/// - "Network" - oznacza operacje sieciowe
/// - "Interface" - oznacza interfejs/abstrakcję
///
/// \see WindowsNetworkInterface, LinuxNetworkInterface
///
////////////////////////////////////////////////////////////
class NetworkInterface {
public:
	////////////////////////////////////////////////////////////
	/// \brief Struktura zawierająca informacje o interfejsie sieciowym
	///
	/// Struktura ta zawiera wszystkie niezbędne informacje
	/// o interfejsie sieciowym, które są wspólne dla wszystkich
	/// platform.
	///
	/// \see NetworkInterface::getInterfaces()
	///
	////////////////////////////////////////////////////////////
	struct InterfaceInfo {
		std::string name;           ///< Nazwa interfejsu
		std::string description;    ///< Opis interfejsu
		std::vector<uint8_t> mac;   ///< Adres MAC (6 bajtów)
		std::vector<uint8_t> ip;    ///< Adres IP (4 bajty)
		uint8_t prefixLength;       ///< Długość prefiksu sieci
		std::vector<uint8_t> gateway; ///< Adres bramy domyślnej
		bool isUp;                  ///< Czy interfejs jest aktywny
	};

	////////////////////////////////////////////////////////////
	/// \brief Wirtualny destruktor
	///
	/// Zapewnia prawidłowe zwolnienie zasobów przy dziedziczeniu.
	///
	////////////////////////////////////////////////////////////
	virtual ~NetworkInterface() = default;

	////////////////////////////////////////////////////////////
	/// \brief Pobiera listę wszystkich interfejsów sieciowych
	///
	/// Funkcja ta zwraca listę wszystkich aktywnych interfejsów
	/// sieciowych dostępnych w systemie.
	///
	/// \return std::vector<InterfaceInfo> Lista interfejsów sieciowych
	///
	/// \see InterfaceInfo
	///
	////////////////////////////////////////////////////////////
	virtual std::vector<InterfaceInfo> getInterfaces() = 0;

	////////////////////////////////////////////////////////////
	/// \brief Rozwiązuje adres IP na adres MAC
	///
	/// Funkcja ta próbuje znaleźć adres MAC odpowiadający
	/// podanemu adresowi IP w tablicy ARP systemu.
	///
	/// \param interfaceName Nazwa interfejsu na którym szukać
	/// \param ip Adres IP do rozstrzygnięcia
	///
	/// \return std::vector<uint8_t> Adres MAC (6 bajtów) lub pusty vector jeśli nie znaleziono
	///
	////////////////////////////////////////////////////////////
	virtual std::vector<uint8_t> resolveMacAddress(const std::string& interfaceName, 
	                                               const std::vector<uint8_t>& ip) = 0;
};

////////////////////////////////////////////////////////////
/// \brief Abstrakcja dla raw socket na różnych platformach
///
/// Klasa ta definiuje interfejs dla operacji raw socket,
/// który może być implementowany na różnych platformach.
/// Umożliwia wysyłanie i odbieranie pakietów sieciowych
/// na poziomie warstwy sieciowej.
///
/// Nazwa klasy "RawSocket" pochodzi od:
/// - "Raw" - oznacza surowe/nieprzetworzone pakiety
/// - "Socket" - oznacza gniazdo sieciowe
///
/// \see WindowsRawSocket, LinuxRawSocket
///
////////////////////////////////////////////////////////////
class RawSocket {
public:
	////////////////////////////////////////////////////////////
	/// \brief Wirtualny destruktor
	///
	/// Zapewnia prawidłowe zwolnienie zasobów przy dziedziczeniu.
	///
	////////////////////////////////////////////////////////////
	virtual ~RawSocket() = default;

	////////////////////////////////////////////////////////////
	/// \brief Otwiera raw socket
	///
	/// Funkcja ta inicjalizuje raw socket na podanym interfejsie.
	///
	/// \param interfaceName Nazwa interfejsu sieciowego
	/// \param promiscuous Czy włączyć tryb promiscuous
	///
	/// \return bool true jeśli socket został pomyślnie otwarty
	///
	/// \see close(), sendPacket(), receivePacket()
	///
	////////////////////////////////////////////////////////////
	virtual bool open(const std::string& interfaceName, bool promiscuous = true) = 0;

	////////////////////////////////////////////////////////////
	/// \brief Zamyka raw socket
	///
	/// Funkcja ta bezpiecznie zamyka raw socket i zwalnia zasoby.
	///
	/// \see open()
	///
	////////////////////////////////////////////////////////////
	virtual void close() = 0;

	////////////////////////////////////////////////////////////
	/// \brief Wysyła pakiet przez raw socket
	///
	/// \param data Dane do wysłania
	///
	/// \return bool true jeśli pakiet został pomyślnie wysłany
	///
	/// \see receivePacket()
	///
	////////////////////////////////////////////////////////////
	virtual bool sendPacket(const std::vector<uint8_t>& data) = 0;

	////////////////////////////////////////////////////////////
	/// \brief Odbiera pakiet z raw socket
	///
	/// \return std::vector<uint8_t> Odebrane dane lub pusty vector jeśli nie ma pakietów
	///
	/// \see sendPacket()
	///
	////////////////////////////////////////////////////////////
	virtual std::vector<uint8_t> receivePacket() = 0;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy socket jest otwarty
	///
	/// \return bool true jeśli socket jest otwarty
	///
	////////////////////////////////////////////////////////////
	virtual bool isOpen() const = 0;
};

////////////////////////////////////////////////////////////
/// \brief Fabryka dla tworzenia implementacji platformowych
///
/// Klasa ta tworzy odpowiednie implementacje dla aktualnej
/// platformy. Umożliwia to dynamiczne wybieranie implementacji
/// w zależności od systemu operacyjnego.
///
/// Nazwa klasy "PlatformFactory" pochodzi od:
/// - "Platform" - oznacza platformę systemową
/// - "Factory" - oznacza wzorzec projektowy fabryki
///
/// \see NetworkInterface, RawSocket
///
////////////////////////////////////////////////////////////
class PlatformFactory {
public:
	////////////////////////////////////////////////////////////
	/// \brief Tworzy implementację NetworkInterface dla aktualnej platformy
	///
	/// \return std::unique_ptr<NetworkInterface> Implementacja dla aktualnej platformy
	///
	/// \see NetworkInterface
	///
	////////////////////////////////////////////////////////////
	static std::unique_ptr<NetworkInterface> createNetworkInterface();

	////////////////////////////////////////////////////////////
	/// \brief Tworzy implementację RawSocket dla aktualnej platformy
	///
	/// \return std::unique_ptr<RawSocket> Implementacja dla aktualnej platformy
	///
	/// \see RawSocket
	///
	////////////////////////////////////////////////////////////
	static std::unique_ptr<RawSocket> createRawSocket();
}; 