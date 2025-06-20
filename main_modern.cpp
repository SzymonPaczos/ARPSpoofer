#include "App.hpp"
#include "IPAddress.hpp"
#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

////////////////////////////////////////////////////////////
/// \brief Obsługa sygnału przerwania (Ctrl+C)
///
/// Funkcja ta jest wywoływana przez system gdy użytkownik
/// naciśnie Ctrl+C w konsoli. Bezpiecznie zatrzymuje
/// główną aplikację.
///
/// \param dwCtrlType Typ sygnału kontrolnego
///
/// \return BOOL TRUE jeśli sygnał został obsłużony
///
////////////////////////////////////////////////////////////
#ifdef _WIN32
BOOL WINAPI CtrlCHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT) {
		App::getInstance().requestStop();
		return TRUE;
	}
	return FALSE;
}
#endif

////////////////////////////////////////////////////////////
/// \brief Wyświetla pomoc programu
///
/// Funkcja ta wyświetla szczegółową pomoc dotyczącą
/// używania programu w nowoczesnym formacie.
///
/// \param programName Nazwa programu
///
////////////////////////////////////////////////////////////
void showHelp(const std::string& programName) {
	std::cout << "ARP Spoofing Tool - Nowoczesna wersja C++\n";
	std::cout << "==========================================\n\n";
	std::cout << "Użycie: " << programName << " [OPCJE] <victim-ip> [target-ip]\n\n";
	std::cout << "Opcje:\n";
	std::cout << "  --help, -h          Wyświetla tę pomoc\n";
	std::cout << "  --list, -l          Wyświetla listę interfejsów sieciowych\n";
	std::cout << "  --interface, -i     Określa interfejs sieciowy\n";
	std::cout << "  --oneway, -o        Tylko jednokierunkowy atak\n";
	std::cout << "  --interval, -t      Interwał pakietów ARP (sekundy, domyślnie 2)\n";
	std::cout << "  --verbose, -v       Szczegółowe logowanie\n\n";
	std::cout << "Argumenty:\n";
	std::cout << "  victim-ip           Adres IP ofiary (wymagany)\n";
	std::cout << "  target-ip           Adres IP celu (opcjonalny, domyślnie brama)\n\n";
	std::cout << "Przykłady:\n";
	std::cout << "  " << programName << " --list\n";
	std::cout << "  " << programName << " 192.168.1.10\n";
	std::cout << "  " << programName << " -i eth0 192.168.1.10\n";
	std::cout << "  " << programName << " --oneway 192.168.1.10\n";
	std::cout << "  " << programName << " -t 5 192.168.1.10 192.168.1.1\n\n";
	std::cout << "UWAGA: Program wymaga uprawnień administratora!\n";
	std::cout << "       Używaj tylko w kontrolowanym środowisku.\n";
}

////////////////////////////////////////////////////////////
/// \brief Parsuje argumenty wiersza poleceń
///
/// Funkcja ta analizuje argumenty i zwraca skonfigurowany
/// obiekt AttackConfig.
///
/// \param argc Liczba argumentów
/// \param argv Tablica argumentów
/// \param config Referencja do konfiguracji do wypełnienia
///
/// \return bool true jeśli parsowanie się powiodło
///
////////////////////////////////////////////////////////////
bool parseArguments(int argc, char* argv[], App::AttackConfig& config) {
	// Domyślne wartości
	config.oneWayMode = false;
	config.arpInterval = 2;
	
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		
		if (arg == "--help" || arg == "-h") {
			showHelp(argv[0]);
			return false;
		}
		else if (arg == "--list" || arg == "-l") {
			App::getInstance().listInterfaces();
			return false;
		}
		else if (arg == "--oneway" || arg == "-o") {
			config.oneWayMode = true;
		}
		else if (arg == "--interval" || arg == "-t") {
			if (i + 1 < argc) {
				try {
					config.arpInterval = std::stoi(argv[++i]);
					if (config.arpInterval <= 0) {
						std::cerr << "Błąd: Interwał musi być większy od 0\n";
						return false;
					}
				} catch (const std::exception&) {
					std::cerr << "Błąd: Nieprawidłowy interwał\n";
					return false;
				}
			} else {
				std::cerr << "Błąd: Brak wartości dla --interval\n";
				return false;
			}
		}
		else if (arg == "--interface" || arg == "-i") {
			if (i + 1 < argc) {
				config.interfaceName = argv[++i];
			} else {
				std::cerr << "Błąd: Brak nazwy interfejsu\n";
				return false;
			}
		}
		else if (arg == "--verbose" || arg == "-v") {
			// TODO: Implementacja verbose mode
		}
		else if (arg[0] != '-') {
			// To jest adres IP
			IPAddress ipAddr = IPAddress::fromString(arg);
			if (ipAddr.isEmpty()) {
				std::cerr << "Błąd: Nieprawidłowy adres IP: " << arg << "\n";
				return false;
			}
			
			if (config.victimIp.isEmpty()) {
				config.victimIp = ipAddr;
			} else if (config.targetIp.isEmpty()) {
				config.targetIp = ipAddr;
			} else {
				std::cerr << "Błąd: Zbyt wiele argumentów\n";
				return false;
			}
		}
		else {
			std::cerr << "Błąd: Nieznana opcja: " << arg << "\n";
			return false;
		}
	}
	
	if (config.victimIp.isEmpty()) {
		std::cerr << "Błąd: Adres IP ofiary jest wymagany\n";
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////
/// \brief Callback dla logowania
///
/// Funkcja callback używana przez aplikację do logowania
/// zdarzeń w konsoli.
///
/// \param level Poziom logowania
/// \param message Wiadomość do zalogowania
///
////////////////////////////////////////////////////////////
void logCallback(int level, const std::string& message) {
	const char* levelStr[] = {"ERROR", "WARN", "INFO", "DEBUG"};
	const char* levelColor[] = {"\033[31m", "\033[33m", "\033[32m", "\033[36m"};
	const char* resetColor = "\033[0m";
	
	if (level >= 0 && level < 4) {
		std::cerr << levelColor[level] << "[" << levelStr[level] << "]" 
		          << resetColor << " " << message << "\n";
	}
}

////////////////////////////////////////////////////////////
/// \brief Callback dla zatrzymania
///
/// Funkcja callback wywoływana gdy atak zostaje zatrzymany.
///
////////////////////////////////////////////////////////////
void stopCallback() {
	std::cout << "\nAtak został zatrzymany.\n";
}

////////////////////////////////////////////////////////////
/// \brief Główna funkcja programu
///
/// Funkcja main() jest punktem wejścia do nowoczesnej
/// wersji aplikacji ARP spoofing. Używa nowoczesnych
/// funkcji C++ i wzorca Singleton.
///
/// \param argc Liczba argumentów
/// \param argv Tablica argumentów
///
/// \return int Kod wyjścia
///
////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	// Sprawdź argumenty
	if (argc < 2) {
		showHelp(argv[0]);
		return 1;
	}
	
	// Parsuj argumenty
	App::AttackConfig config;
	if (!parseArguments(argc, argv, config)) {
		return 1;
	}
	
	// Pobierz instancję aplikacji
	App& app = App::getInstance();
	
	// Ustaw callbacks
	app.setLogCallback(logCallback);
	app.setStopCallback(stopCallback);
	
	// Zarejestruj obsługę sygnału Ctrl+C
#ifdef _WIN32
	SetConsoleCtrlHandler(CtrlCHandler, TRUE);
#endif
	
	// Skonfiguruj atak
	if (!app.configureAttack(config)) {
		return 1;
	}
	
	// Rozpocznij atak
	if (!app.startAttack()) {
		return 1;
	}
	
	return 0;
} 