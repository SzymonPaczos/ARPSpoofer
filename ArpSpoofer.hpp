#pragma once

#include "IPAddress.hpp"
#include "RawSocket.hpp"
#include "NetworkHeaders.hpp"
#include "NetworkUtils.hpp"
#include <atomic>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <thread>
#include <functional>

////////////////////////////////////////////////////////////
/// \brief Główna klasa odpowiedzialna za przeprowadzenie ataku ARP spoofing
///
/// Klasa ta koordynuje cały proces ataku ARP spoofing, od wykrycia interfejsów
/// sieciowych, przez rozstrzygnięcie adresów MAC, aż po wysyłanie fałszywych
/// pakietów ARP i przekazywanie ruchu sieciowego. Jest to centralny punkt
/// całej aplikacji.
///
/// Nazwa klasy "ArpSpoofer" pochodzi od:
/// - "Arp" - oznacza protokół Address Resolution Protocol
/// - "Spoofer" - oznacza fałszowanie/oszustwo
///
/// Klasa implementuje wzorzec projektowy Singleton, zapewniając
/// że istnieje tylko jedna instancja w całej aplikacji.
///
/// \see InterfaceManager, RawSocket, NetworkUtils
///
////////////////////////////////////////////////////////////
class ArpSpoofer {
private:
	std::unique_ptr<RawSocket> socket; ///< Gniazdo do wysyłania pakietów ARP
	
	IPAddress victimIp;                ///< Adres IP ofiary
	IPAddress targetIp;                ///< Adres IP celu
	uint8_t victimMac[6];              ///< Adres MAC ofiary
	uint8_t targetMac[6];              ///< Adres MAC celu
	uint8_t myMac[6];                  ///< Nasz adres MAC
	
	bool oneWayMode;                   ///< Tryb jednokierunkowy
	
	std::atomic<bool> running;         ///< Flaga oznaczająca czy atak jest aktywny
	std::thread spoofThread;           ///< Wątek wykonujący atak ARP
	
	std::function<void(const std::string&)> logCallback; ///< Callback do logowania

public:
	////////////////////////////////////////////////////////////
	/// \brief Konstruktor
	///
	/// Tworzy nową instancję ArpSpoofer z podanymi parametrami.
	///
	/// \param victimIp Adres IP ofiary
	/// \param targetIp Adres IP celu
	/// \param oneWayMode Tryb jednokierunkowy (true = oszukuj tylko ofiarę)
	///
	/// \see setVictimMac(), setTargetMac(), setMyMac()
	///
	////////////////////////////////////////////////////////////
	ArpSpoofer(const IPAddress& victimIp, const IPAddress& targetIp, bool oneWayMode = false);

	////////////////////////////////////////////////////////////
	/// \brief Destruktor
	///
	/// Automatycznie zatrzymuje atak i zwalnia zasoby.
	///
	/// \see stop()
	///
	////////////////////////////////////////////////////////////
	~ArpSpoofer();

	////////////////////////////////////////////////////////////
	/// \brief Ustawia adres MAC ofiary
	///
	/// \param mac Tablica 6 bajtów reprezentująca adres MAC
	///
	/// \see setTargetMac(), setMyMac()
	///
	////////////////////////////////////////////////////////////
	void setVictimMac(const uint8_t mac[6]);

	////////////////////////////////////////////////////////////
	/// \brief Ustawia adres MAC celu
	///
	/// \param mac Tablica 6 bajtów reprezentująca adres MAC
	///
	/// \see setVictimMac(), setMyMac()
	///
	////////////////////////////////////////////////////////////
	void setTargetMac(const uint8_t mac[6]);

	////////////////////////////////////////////////////////////
	/// \brief Ustawia nasz adres MAC
	///
	/// \param mac Tablica 6 bajtów reprezentująca adres MAC
	///
	/// \see setVictimMac(), setTargetMac()
	///
	////////////////////////////////////////////////////////////
	void setMyMac(const uint8_t mac[6]);

	////////////////////////////////////////////////////////////
	/// \brief Ustawia callback do logowania
	///
	/// \param callback Funkcja do wywołania przy logowaniu
	///
	/// \see log()
	///
	////////////////////////////////////////////////////////////
	void setLogCallback(std::function<void(const std::string&)> callback);

	////////////////////////////////////////////////////////////
	/// \brief Rozpoczyna atak ARP spoofing
	///
	/// Uruchamia wątek, który będzie wysyłał fałszywe pakiety ARP.
	///
	/// \return bool true jeśli atak został rozpoczęty pomyślnie
	///
	/// \see stop(), isRunning()
	///
	////////////////////////////////////////////////////////////
	bool start();

	////////////////////////////////////////////////////////////
	/// \brief Zatrzymuje atak ARP spoofing
	///
	/// Zatrzymuje wątek ataku i czeka na jego zakończenie.
	///
	/// \see start(), isRunning()
	///
	////////////////////////////////////////////////////////////
	void stop();

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy atak jest aktywny
	///
	/// \return bool true jeśli atak jest w trakcie wykonywania
	///
	/// \see start(), stop()
	///
	////////////////////////////////////////////////////////////
	bool isRunning() const { return running.load(); }

	////////////////////////////////////////////////////////////
	/// \brief Wysyła pojedynczy pakiet ARP spoof
	///
	/// Wysyła fałszywy pakiet ARP do ofiary i/lub celu.
	///
	/// \return bool true jeśli pakiet został wysłany pomyślnie
	///
	/// \see start()
	///
	////////////////////////////////////////////////////////////
	bool sendSpoofPacket();

	////////////////////////////////////////////////////////////
	/// \brief Wysyła pakiet ARP reply
	///
	/// Wysyła pakiet ARP reply informujący, że nasz MAC odpowiada
	/// za podany adres IP.
	///
	/// \param targetIp Adres IP, za który odpowiadamy
	/// \param targetMac Adres MAC odbiorcy
	/// \param spoofedIp Adres IP, który fałszujemy
	///
	/// \return bool true jeśli pakiet został wysłany pomyślnie
	///
	/// \see sendSpoofPacket()
	///
	////////////////////////////////////////////////////////////
	bool sendArpReply(const IPAddress& targetIp, const uint8_t targetMac[6], const IPAddress& spoofedIp);

	////////////////////////////////////////////////////////////
	/// \brief Wysyła pakiet ARP request
	///
	/// Wysyła pakiet ARP request w celu odkrycia adresu MAC.
	///
	/// \param targetIp Adres IP, którego MAC chcemy odkryć
	///
	/// \return bool true jeśli pakiet został wysłany pomyślnie
	///
	/// \see sendArpReply()
	///
	////////////////////////////////////////////////////////////
	bool sendArpRequest(const IPAddress& targetIp);

	////////////////////////////////////////////////////////////
	/// \brief Pobiera adres IP ofiary
	///
	/// \return IPAddress Adres IP ofiary
	///
	/// \see getTargetIp()
	///
	////////////////////////////////////////////////////////////
	IPAddress getVictimIp() const { return victimIp; }

	////////////////////////////////////////////////////////////
	/// \brief Pobiera adres IP celu
	///
	/// \return IPAddress Adres IP celu
	///
	/// \see getVictimIp()
	///
	////////////////////////////////////////////////////////////
	IPAddress getTargetIp() const { return targetIp; }

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy tryb jednokierunkowy jest włączony
	///
	/// \return bool true jeśli tryb jednokierunkowy jest aktywny
	///
	/// \see ArpSpoofer()
	///
	////////////////////////////////////////////////////////////
	bool isOneWayMode() const { return oneWayMode; }

private:
	////////////////////////////////////////////////////////////
	/// \brief Główna pętla ataku ARP
	///
	/// Funkcja wykonywana w osobnym wątku, która wysyła
	/// fałszywe pakiety ARP w regularnych odstępach czasu.
	///
	/// \see start(), stop()
	///
	////////////////////////////////////////////////////////////
	void spoofLoop();

	////////////////////////////////////////////////////////////
	/// \brief Loguje wiadomość
	///
	/// Wywołuje callback logowania jeśli jest ustawiony.
	///
	/// \param message Wiadomość do zalogowania
	///
	/// \see setLogCallback()
	///
	////////////////////////////////////////////////////////////
	void log(const std::string& message) const;

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy wszystkie wymagane dane są ustawione
	///
	/// \return bool true jeśli wszystkie dane są gotowe do ataku
	///
	/// \see start()
	///
	////////////////////////////////////////////////////////////
	bool isReady() const;
}; 