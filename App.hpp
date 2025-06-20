#pragma once

#include "PlatformAbstraction.hpp"
#include "IPAddress.hpp"
#include "NetworkHeaders.hpp"
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <functional>

////////////////////////////////////////////////////////////
/// \brief Główna klasa aplikacji ARP spoofing
///
/// Klasa ta jest centralnym punktem aplikacji, odpowiedzialnym
/// za koordynację wszystkich operacji ARP spoofing. Implementuje
/// wzorzec Singleton, zapewniając że istnieje tylko jedna
/// instancja aplikacji w całym programie.
///
/// Nazwa klasy "App" pochodzi od:
/// - "App" - skrót od "Application" (aplikacja)
///
/// Klasa używa nowoczesnych funkcji C++ takich jak:
/// - std::unique_ptr dla zarządzania zasobami
/// - std::atomic dla bezpiecznej komunikacji między wątkami
/// - std::function dla callbacków
/// - RAII dla automatycznego zarządzania zasobami
///
/// \see NetworkInterface, RawSocket, PlatformFactory, IPAddress
///
////////////////////////////////////////////////////////////
class App {
public:
	////////////////////////////////////////////////////////////
	/// \brief Struktura konfiguracji ataku
	///
	/// Struktura ta zawiera wszystkie parametry potrzebne
	/// do przeprowadzenia ataku ARP spoofing.
	///
	/// \see configureAttack()
	///
	////////////////////////////////////////////////////////////
	struct AttackConfig {
		IPAddress victimIp;         ///< Adres IP ofiary
		IPAddress targetIp;         ///< Adres IP celu (opcjonalny)
		std::string interfaceName;  ///< Nazwa interfejsu (opcjonalny)
		bool oneWayMode;            ///< Tryb jednokierunkowy
		int arpInterval;            ///< Interwał wysyłania pakietów ARP (sekundy)
	};

	////////////////////////////////////////////////////////////
	/// \brief Struktura informacji o ataku
	///
	/// Struktura ta zawiera informacje o aktualnie
	/// przeprowadzanym ataku.
	///
	/// \see getAttackInfo()
	///
	////////////////////////////////////////////////////////////
	struct AttackInfo {
		IPAddress victimIp;         ///< Adres IP ofiary
		IPAddress targetIp;         ///< Adres IP celu
		std::vector<uint8_t> victimMac;   ///< Adres MAC ofiary
		std::vector<uint8_t> targetMac;   ///< Adres MAC celu
		std::vector<uint8_t> myMac;       ///< Nasz adres MAC
		std::string interfaceName;        ///< Nazwa interfejsu
		bool isActive;                    ///< Czy atak jest aktywny
		uint64_t packetsSent;             ///< Liczba wysłanych pakietów
		uint64_t packetsReceived;         ///< Liczba odebranych pakietów
	};

	////////////////////////////////////////////////////////////
	/// \brief Typ callback dla logowania
	///
	/// Definicja typu funkcji callback używanej do logowania
	/// zdarzeń w aplikacji.
	///
	/// \param level Poziom logowania (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
	/// \param message Wiadomość do zalogowania
	///
	////////////////////////////////////////////////////////////
	using LogCallback = std::function<void(int level, const std::string& message)>;

	////////////////////////////////////////////////////////////
	/// \brief Typ callback dla sygnału zatrzymania
	///
	/// Definicja typu funkcji callback używanej do obsługi
	/// sygnału zatrzymania ataku.
	///
	////////////////////////////////////////////////////////////
	using StopCallback = std::function<void()>;

private:
	std::unique_ptr<NetworkInterface> networkInterface; ///< Interfejs sieciowy
	std::unique_ptr<RawSocket> rawSocket;               ///< Raw socket
	std::atomic<bool> stopFlag;                         ///< Flaga zatrzymania
	std::atomic<bool> isRunning;                        ///< Czy aplikacja działa
	
	AttackConfig config;                                ///< Konfiguracja ataku
	AttackInfo attackInfo;                              ///< Informacje o ataku
	
	LogCallback logCallback;                            ///< Callback dla logowania
	StopCallback stopCallback;                          ///< Callback dla zatrzymania

	// Singleton pattern
	static std::unique_ptr<App> instance;               ///< Instancja singleton
	App();                                              ///< Prywatny konstruktor

public:
	////////////////////////////////////////////////////////////
	/// \brief Destruktor
	///
	/// Automatycznie zatrzymuje atak i zwalnia zasoby
	/// gdy obiekt App jest niszczony.
	///
	/// \see stopAttack()
	///
	////////////////////////////////////////////////////////////
	~App();

	////////////////////////////////////////////////////////////
	/// \brief Pobiera instancję singleton
	///
	/// Funkcja ta zwraca referencję do jedynej instancji
	/// klasy App (wzorzec Singleton).
	///
	/// \return App& Referencja do instancji aplikacji
	///
	/// \see ~App()
	///
	////////////////////////////////////////////////////////////
	static App& getInstance();

	////////////////////////////////////////////////////////////
	/// \brief Wyświetla listę dostępnych interfejsów sieciowych
	///
	/// Funkcja ta pobiera i wyświetla wszystkie aktywne
	/// interfejsy sieciowe dostępne w systemie.
	///
	/// \return bool true jeśli operacja się powiodła
	///
	/// \see NetworkInterface::getInterfaces()
	///
	////////////////////////////////////////////////////////////
	bool listInterfaces();

	////////////////////////////////////////////////////////////
	/// \brief Konfiguruje parametry ataku ARP spoofing
	///
	/// Funkcja ta ustawia wszystkie parametry potrzebne
	/// do przeprowadzenia ataku: adresy IP ofiary i celu,
	/// interfejs sieciowy oraz tryb działania.
	///
	/// \param config Konfiguracja ataku
	///
	/// \return bool true jeśli konfiguracja się powiodła
	///
	/// \see AttackConfig, startAttack()
	///
	////////////////////////////////////////////////////////////
	bool configureAttack(const AttackConfig& config);

	////////////////////////////////////////////////////////////
	/// \brief Rozpoczyna atak ARP spoofing
	///
	/// Funkcja ta rozpoczyna główną pętlę ataku, która:
	/// 1. Wysyła fałszywe pakiety ARP w określonych interwałach
	/// 2. Przechwytuje i przekazuje ruch sieciowy
	/// 3. Obsługuje sygnał przerwania
	///
	/// Funkcja działa w pętli aż do wywołania stopAttack()
	/// lub ustawienia flagi zatrzymania.
	///
	/// \return bool true jeśli atak zakończył się pomyślnie
	///
	/// \see stopAttack(), configureAttack()
	///
	////////////////////////////////////////////////////////////
	bool startAttack();

	////////////////////////////////////////////////////////////
	/// \brief Zatrzymuje atak ARP spoofing
	///
	/// Funkcja ta bezpiecznie zatrzymuje atak, wysyłając
	/// prawidłowe pakiety ARP aby przywrócić normalne
	/// działanie sieci.
	///
	/// \see startAttack(), configureAttack()
	///
	////////////////////////////////////////////////////////////
	void stopAttack();

	////////////////////////////////////////////////////////////
	/// \brief Ustawia flagę zatrzymania ataku
	///
	/// Funkcja ta jest wywoływana przez obsługę sygnału
	/// aby bezpiecznie zatrzymać główną pętlę ataku.
	///
	/// \see startAttack(), stopAttack()
	///
	////////////////////////////////////////////////////////////
	void requestStop() { stopFlag = true; }

	////////////////////////////////////////////////////////////
	/// \brief Pobiera informacje o aktualnym ataku
	///
	/// \return const AttackInfo& Referencja do informacji o ataku
	///
	/// \see AttackInfo
	///
	////////////////////////////////////////////////////////////
	const AttackInfo& getAttackInfo() const { return attackInfo; }

	////////////////////////////////////////////////////////////
	/// \brief Sprawdza czy atak jest aktywny
	///
	/// \return bool true jeśli atak jest w trakcie wykonywania
	///
	/// \see startAttack(), stopAttack()
	///
	////////////////////////////////////////////////////////////
	bool isAttackActive() const { return isRunning; }

	////////////////////////////////////////////////////////////
	/// \brief Ustawia callback dla logowania
	///
	/// \param callback Funkcja callback do logowania
	///
	/// \see LogCallback
	///
	////////////////////////////////////////////////////////////
	void setLogCallback(LogCallback callback) { logCallback = std::move(callback); }

	////////////////////////////////////////////////////////////
	/// \brief Ustawia callback dla zatrzymania
	///
	/// \param callback Funkcja callback dla zatrzymania
	///
	/// \see StopCallback
	///
	////////////////////////////////////////////////////////////
	void setStopCallback(StopCallback callback) { stopCallback = std::move(callback); }

private:
	////////////////////////////////////////////////////////////
	/// \brief Tworzy pakiet ARP spoofing
	///
	/// Funkcja ta konstruuje kompletny pakiet ARP spoofing
	/// składający się z nagłówka Ethernet i nagłówka ARP.
	///
	/// \param victimIp Adres IP ofiary
	/// \param victimMac Adres MAC ofiary
	/// \param spoofedIp Adres IP który fałszujemy
	/// \param myMac Nasz adres MAC
	///
	/// \return std::vector<uint8_t> Skonstruowany pakiet
	///
	/// \see startAttack()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> createArpPacket(const IPAddress& victimIp,
	                                     const std::vector<uint8_t>& victimMac,
	                                     const IPAddress& spoofedIp,
	                                     const std::vector<uint8_t>& myMac);

	////////////////////////////////////////////////////////////
	/// \brief Przetwarza odebrane pakiety
	///
	/// Funkcja ta analizuje odebrane pakiety i decyduje
	/// czy należy je przekazać dalej.
	///
	/// \param data Dane pakietu do przetworzenia
	///
	/// \see startAttack()
	///
	////////////////////////////////////////////////////////////
	void handlePacket(const std::vector<uint8_t>& data);

	////////////////////////////////////////////////////////////
	/// \brief Loguje wiadomość
	///
	/// Wywołuje callback logowania jeśli jest ustawiony.
	///
	/// \param level Poziom logowania
	/// \param message Wiadomość do zalogowania
	///
	/// \see setLogCallback()
	///
	////////////////////////////////////////////////////////////
	void log(int level, const std::string& message);
}; 