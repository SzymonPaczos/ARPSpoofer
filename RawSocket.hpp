#pragma once

#include <string>
#include <cstdint>
#include <winsock2.h>

////////////////////////////////////////////////////////////
/// \brief Klasa do obsługi raw socket dla operacji sieciowych
///
/// Klasa ta enkapsuluje funkcjonalność raw socket w Windows,
/// umożliwiając wysyłanie i odbieranie pakietów sieciowych
/// na poziomie warstwy sieciowej. Jest to alternatywa dla
/// biblioteki WinPcap/Npcap, używająca tylko standardowych
/// Windows API.
///
/// Nazwa klasy "RawSocket" pochodzi od:
/// - "Raw" - oznacza surowe/nieprzetworzone pakiety sieciowe
/// - "Socket" - oznacza gniazdo sieciowe
///
/// Klasa obsługuje inicjalizację Winsock, tworzenie raw socket,
/// ustawienie trybu promiscuous oraz bezpieczne zamykanie połączenia.
///
/// \see ArpSpoofer, PacketHandler
///
////////////////////////////////////////////////////////////
class RawSocket {
private:
	SOCKET sock;        ///< Deskryptor gniazda sieciowego
	bool promiscuous;   ///< Flaga określająca czy tryb promiscuous jest włączony

public:
	////////////////////////////////////////////////////////////
	/// \brief Konstruktor domyślny
	///
	/// Inicjalizuje obiekt RawSocket z domyślnymi wartościami.
	/// Gniazdo jest ustawione na INVALID_SOCKET, a tryb promiscuous
	/// jest wyłączony.
	///
	////////////////////////////////////////////////////////////
	RawSocket();

	////////////////////////////////////////////////////////////
	/// \brief Destruktor
	///
	/// Automatycznie zamyka raw socket i zwalnia zasoby
	/// gdy obiekt RawSocket jest niszczony.
	///
	/// \see close()
	///
	////////////////////////////////////////////////////////////
	~RawSocket();

	////////////////////////////////////////////////////////////
	/// \brief Otwiera raw socket i inicjalizuje połączenie sieciowe
	///
	/// Funkcja ta inicjalizuje bibliotekę Winsock, tworzy raw socket
	/// oraz opcjonalnie włącza tryb promiscuous. Jest to pierwszy
	/// krok przed rozpoczęciem operacji sieciowych.
	///
	/// \param interface_name Nazwa interfejsu sieciowego (nie używana w tej implementacji)
	/// \param promisc Flaga określająca czy włączyć tryb promiscuous
	///
	/// \return bool true jeśli socket został pomyślnie otwarty,
	///         false w przypadku błędu (brak uprawnień administratora)
	///
	/// \see close(), sendPacket(), receivePacket()
	///
	////////////////////////////////////////////////////////////
	bool open(const std::string& interface_name, bool promisc = true);

	////////////////////////////////////////////////////////////
	/// \brief Wysyła pakiet przez raw socket
	///
	/// Funkcja ta wysyła dane przez raw socket do sieci.
	/// Dla pakietów ARP używana jest funkcja sendto() z adresem
	/// docelowym ustawionym na INADDR_ANY.
	///
	/// \param data Wskaźnik na dane do wysłania
	/// \param len Długość danych w bajtach
	///
	/// \return bool true jeśli pakiet został pomyślnie wysłany,
	///         false w przypadku błędu
	///
	/// \see receivePacket(), open()
	///
	////////////////////////////////////////////////////////////
	bool sendPacket(const uint8_t* data, size_t len);

	////////////////////////////////////////////////////////////
	/// \brief Odbiera pakiet z raw socket
	///
	/// Funkcja ta próbuje odebrać pakiet z raw socket.
	/// Jeśli nie ma dostępnych pakietów, funkcja zwraca false.
	/// Odebrane dane są zapisywane do podanego bufora.
	///
	/// \param buffer Bufor do zapisania odebranych danych
	/// \param buffer_size Rozmiar bufora w bajtach
	/// \param received_size Referencja do zmiennej która otrzyma rzeczywistą długość odebranych danych
	///
	/// \return bool true jeśli pakiet został pomyślnie odebrany,
	///         false jeśli nie ma pakietów lub wystąpił błąd
	///
	/// \see sendPacket(), open()
	///
	////////////////////////////////////////////////////////////
	bool receivePacket(uint8_t* buffer, size_t buffer_size, size_t& received_size);

	////////////////////////////////////////////////////////////
	/// \brief Zamyka raw socket i zwalnia zasoby
	///
	/// Funkcja ta bezpiecznie zamyka raw socket i zwalnia
	/// zasoby biblioteki Winsock. Powinna być wywołana
	/// przed zniszczeniem obiektu RawSocket.
	///
	/// \see open(), ~RawSocket()
	///
	////////////////////////////////////////////////////////////
	void close();

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy socket jest otwarty
	///
	/// \return bool true jeśli socket jest otwarty i gotowy do użycia,
	///         false w przeciwnym przypadku
	///
	/// \see open(), close()
	///
	////////////////////////////////////////////////////////////
	bool isOpen() const { return sock != INVALID_SOCKET; }
}; 