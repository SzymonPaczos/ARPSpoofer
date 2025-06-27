#include "App.hpp"
#include "IPAddress.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

////////////////////////////////////////////////////////////
/// \brief Interrupt signal handler (Ctrl+C)
///
/// This function is called by the system when the user
/// presses Ctrl+C in the console. Safely stops the
/// main application.
///
/// \param dwCtrlType Control signal type
///
/// \return BOOL TRUE if signal was handled
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
/// \brief Displays program help
///
/// This function displays detailed help about
/// using the program in modern format.
///
/// \param programName Program name
///
////////////////////////////////////////////////////////////
void showHelp(const std::string& programName) {
	std::cout << "ARP Spoofing Tool - C++\n";
	std::cout << "========================\n\n";
	std::cout << "Usage: " << programName << " [OPTIONS] <victim-ip> [target-ip]\n";
	std::cout << "   lub: " << programName << " (tryb interaktywny)\n\n";
	std::cout << "Options:\n";
	std::cout << "  --help, -h          Display this help\n";
	std::cout << "  --list, -l          Display list of network interfaces\n";
	std::cout << "  --interface, -i     Specify network interface\n";
	std::cout << "  --oneway, -o        One-way attack only\n";
	std::cout << "  --drop, -d          Drop packets instead of forwarding (cuts internet)\n";
	std::cout << "  --interval, -t      ARP packet interval (seconds, default 2)\n";
	std::cout << "  --verbose, -v       Detailed logging\n\n";
	std::cout << "Arguments:\n";
	std::cout << "  victim-ip           Victim's IP address (required)\n";
	std::cout << "  target-ip           Target's IP address (optional, default gateway)\n\n";
	std::cout << "Interactive Mode:\n";
	std::cout << "  Uruchom program bez argumentów aby przejść do trybu interaktywnego.\n";
	std::cout << "  Program poprowadzi Cię przez proces konfiguracji krok po kroku.\n\n";
	std::cout << "Examples:\n";
	std::cout << "  " << programName << " --list\n";
	std::cout << "  " << programName << " 192.168.1.10\n";
	std::cout << "  " << programName << " -i eth0 192.168.1.10\n";
	std::cout << "  " << programName << " --oneway 192.168.1.10\n";
	std::cout << "  " << programName << " --drop 192.168.1.10 (odcina internet)\n";
	std::cout << "  " << programName << " -t 5 192.168.1.10 192.168.1.1\n";
	std::cout << "  " << programName << " (tryb interaktywny)\n\n";
	std::cout << "WARNING: Program requires administrator privileges!\n";
	std::cout << "         Use only in controlled environment.\n";
	std::cout << "\n";
	std::cout << "REQUIRED PRIVILEGES:\n";
	std::cout << "  - Raw socket access (for ARP packet injection)\n";
	std::cout << "  - Promiscuous mode (for network traffic interception)\n";
	std::cout << "\n";
	std::cout << "HOW TO RUN:\n";
#ifdef _WIN32
	std::cout << "  Windows: Run PowerShell/Command Prompt as Administrator\n";
	std::cout << "           Then execute: " << programName << "\n";
#else
	std::cout << "  Linux:   sudo " << programName << "\n";
	std::cout << "  Or set capabilities: sudo setcap cap_net_raw,cap_net_admin=eip " << programName << "\n";
#endif
}

////////////////////////////////////////////////////////////
/// \brief Parses command line arguments
///
/// This function analyzes arguments and returns configured
/// AttackConfig object.
///
/// \param argc Number of arguments
/// \param argv Array of arguments
/// \param config Reference to configuration to fill
///
/// \return bool true if parsing succeeded
///
////////////////////////////////////////////////////////////
bool parseArguments(int argc, char* argv[], App::AttackConfig& config) {
	// Default values
	config.oneWayMode = false;
	config.dropMode = false;
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
						std::cerr << "Error: Interval must be greater than 0\n";
						return false;
					}
				} catch (const std::exception&) {
					std::cerr << "Error: Invalid interval\n";
					return false;
				}
			} else {
				std::cerr << "Error: Missing value for --interval\n";
				return false;
			}
		}
		else if (arg == "--interface" || arg == "-i") {
			if (i + 1 < argc) {
				config.interfaceName = argv[++i];
			} else {
				std::cerr << "Error: Missing interface name\n";
				return false;
			}
		}
		else if (arg == "--verbose" || arg == "-v") {
			// TODO: Implement verbose mode
		}
		else if (arg == "--drop" || arg == "-d") {
			config.dropMode = true;
		}
		else if (arg[0] != '-') {
			// This is an IP address
			IPAddress ipAddr = IPAddress::fromString(arg);
			if (ipAddr.isEmpty()) {
				std::cerr << "Error: Invalid IP address: " << arg << "\n";
				return false;
			}
			
			if (config.victimIp.isEmpty()) {
				config.victimIp = ipAddr;
			} else if (config.targetIp.isEmpty()) {
				config.targetIp = ipAddr;
			} else {
				std::cerr << "Error: Too many arguments\n";
				return false;
			}
		}
		else {
			std::cerr << "Error: Unknown option: " << arg << "\n";
			return false;
		}
	}
	
	if (config.victimIp.isEmpty()) {
		std::cerr << "Error: Victim IP address is required\n";
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////
/// \brief Interactive parameter input
///
/// This function allows user to input attack parameters
/// interactively when no command line arguments are provided.
///
/// \param config Reference to configuration to fill
///
/// \return bool true if input succeeded
///
////////////////////////////////////////////////////////////
bool interactiveInput(App::AttackConfig& config) {
	std::cout << "\n=== Interaktywne wprowadzanie parametrów ===\n\n";
	
	// Default values
	config.oneWayMode = false;
	config.dropMode = false;
	config.arpInterval = 2;
	
	// Victim IP
	std::string victimIpStr;
	do {
		std::cout << "Podaj adres IP ofiary: ";
		std::getline(std::cin, victimIpStr);
		
		config.victimIp = IPAddress::fromString(victimIpStr);
		if (config.victimIp.isEmpty()) {
			std::cout << "Błąd: Nieprawidłowy adres IP. Spróbuj ponownie.\n";
		}
	} while (config.victimIp.isEmpty());
	
	// Target IP (optional)
	std::cout << "Podaj adres IP celu (Enter = brama domyślna): ";
	std::string targetIpStr;
	std::getline(std::cin, targetIpStr);
	
	if (!targetIpStr.empty()) {
		config.targetIp = IPAddress::fromString(targetIpStr);
		if (config.targetIp.isEmpty()) {
			std::cout << "Ostrzeżenie: Nieprawidłowy adres IP celu. Użyję bramy domyślnej.\n";
			config.targetIp = IPAddress(); // Empty = use default gateway
		}
	}
	
	// Interface (optional)
	std::cout << "Podaj nazwę interfejsu (Enter = automatyczne wykrycie, 'l' = lista interfejsów): ";
	std::getline(std::cin, config.interfaceName);
	
	if (config.interfaceName == "l" || config.interfaceName == "L") {
		App::getInstance().listInterfaces();
		std::cout << "\nPodaj nazwę interfejsu (Enter = automatyczne wykrycie): ";
		std::getline(std::cin, config.interfaceName);
	}
	
	// One-way mode
	std::string oneWayStr;
	do {
		std::cout << "Tryb jednokierunkowy? (t/n, domyślnie n): ";
		std::getline(std::cin, oneWayStr);
		
		if (oneWayStr.empty()) {
			config.oneWayMode = false;
			break;
		} else if (oneWayStr == "t" || oneWayStr == "T" || oneWayStr == "tak" || oneWayStr == "TAK") {
			config.oneWayMode = true;
			break;
		} else if (oneWayStr == "n" || oneWayStr == "N" || oneWayStr == "nie" || oneWayStr == "NIE") {
			config.oneWayMode = false;
			break;
		} else {
			std::cout << "Błąd: Wprowadź 't' lub 'n'.\n";
		}
	} while (true);
	
	// Drop mode
	std::string dropStr;
	do {
		std::cout << "Czy chcesz odrzucić pakiety? (t/n, domyślnie n): ";
		std::cout << "UWAGA: To odetnie internet między urządzeniami! ";
		std::getline(std::cin, dropStr);
		
		if (dropStr.empty()) {
			config.dropMode = false;
			break;
		} else if (dropStr == "t" || dropStr == "T" || dropStr == "tak" || dropStr == "TAK") {
			std::cout << "OSTRZEŻENIE: Tryb porzucania pakietów odetnie internet!\n";
			std::cout << "Czy na pewno chcesz kontynuować? (t/n): ";
			std::string confirm;
			std::getline(std::cin, confirm);
			if (confirm == "t" || confirm == "T" || confirm == "tak" || confirm == "TAK") {
				config.dropMode = true;
				break;
			} else {
				config.dropMode = false;
				break;
			}
		} else if (dropStr == "n" || dropStr == "N" || dropStr == "nie" || dropStr == "NIE") {
			config.dropMode = false;
			break;
		} else {
			std::cout << "Błąd: Wprowadź 't' lub 'n'.\n";
		}
	} while (true);
	
	// Interval
	std::string intervalStr;
	do {
		std::cout << "Interwał pakietów ARP w sekundach (Enter = 2): ";
		std::getline(std::cin, intervalStr);
		
		if (intervalStr.empty()) {
			config.arpInterval = 2;
			break;
		} else {
			try {
				int interval = std::stoi(intervalStr);
				if (interval > 0) {
					config.arpInterval = interval;
					break;
				} else {
					std::cout << "Błąd: Interwał musi być większy od 0.\n";
				}
			} catch (const std::exception&) {
				std::cout << "Błąd: Nieprawidłowa wartość. Wprowadź liczbę.\n";
			}
		}
	} while (true);
	
	// Show summary
	std::cout << "\n=== Podsumowanie konfiguracji ===\n";
	std::cout << "Ofiara: " << config.victimIp.toString() << "\n";
	if (!config.targetIp.isEmpty()) {
		std::cout << "Cel: " << config.targetIp.toString() << "\n";
	} else {
		std::cout << "Cel: brama domyślna (automatyczne wykrycie)\n";
	}
	if (!config.interfaceName.empty()) {
		std::cout << "Interfejs: " << config.interfaceName << "\n";
	} else {
		std::cout << "Interfejs: automatyczne wykrycie\n";
	}
	std::cout << "Tryb jednokierunkowy: " << (config.oneWayMode ? "tak" : "nie") << "\n";
	std::cout << "Odrzucanie pakietów: " << (config.dropMode ? "tak" : "nie") << "\n";
	std::cout << "Interwał ARP: " << config.arpInterval << " sekund\n";
	
	// Confirm
	std::string confirmStr;
	do {
		std::cout << "\nCzy chcesz rozpocząć atak? (t/n): ";
		std::getline(std::cin, confirmStr);
		
		if (confirmStr == "t" || confirmStr == "T" || confirmStr == "tak" || confirmStr == "TAK") {
			return true;
		} else if (confirmStr == "n" || confirmStr == "N" || confirmStr == "nie" || confirmStr == "NIE") {
			return false;
		} else {
			std::cout << "Błąd: Wprowadź 't' lub 'n'.\n";
		}
	} while (true);
}

////////////////////////////////////////////////////////////
/// \brief Logging callback
///
/// Callback function used by application to log
/// events to console.
///
/// \param level Log level
/// \param message Message to log
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
/// \brief Stop callback
///
/// Callback function called when attack is stopped.
///
////////////////////////////////////////////////////////////
void stopCallback() {
	std::cout << "\nAttack has been stopped.\n";
}

////////////////////////////////////////////////////////////
/// \brief Main program function
///
/// The main() function is the entry point to the modern
/// ARP spoofing application. Uses modern C++ features
/// and Singleton pattern.
///
/// \param argc Number of arguments
/// \param argv Array of arguments
///
/// \return int Exit code
///
////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	// Check arguments
	if (argc < 2) {
		showHelp(argv[0]);
		std::cout << "\nNie podano argumentów. Czy chcesz wprowadzić parametry interaktywnie? (t/n): ";
		
		std::string choice;
		std::getline(std::cin, choice);
		
		if (choice == "t" || choice == "T" || choice == "tak" || choice == "TAK") {
			App::AttackConfig config;
			if (!interactiveInput(config)) {
				std::cout << "Anulowano.\n";
				return 0;
			}
			
			// Get application instance
			App& app = App::getInstance();
			
			// Set up callbacks
			app.setLogCallback(logCallback);
			app.setStopCallback(stopCallback);
			
			// Register Ctrl+C handler
		#ifdef _WIN32
			SetConsoleCtrlHandler(CtrlCHandler, TRUE);
		#endif
			
			// Configure attack
			if (!app.configureAttack(config)) {
				std::cerr << "Error: Failed to configure attack\n";
				return 1;
			}
			
			// Start attack
			std::cout << "Starting ARP spoofing attack...\n";
			std::cout << "Press Ctrl+C to stop\n\n";
			
			if (!app.startAttack()) {
				std::cerr << "Error: Attack failed\n";
				return 1;
			}
			
			return 0;
		} else {
			std::cout << "Użyj argumentów wiersza poleceń lub uruchom ponownie bez argumentów.\n";
			return 1;
		}
	}

	// Handle --help option
	if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
		showHelp(argv[0]);
		return 0;
	}

	// Parse arguments
	App::AttackConfig config;
	if (!parseArguments(argc, argv, config)) {
		return 1;
	}

	// Get application instance
	App& app = App::getInstance();

	// Set up callbacks
	app.setLogCallback(logCallback);
	app.setStopCallback(stopCallback);

	// Register Ctrl+C handler
#ifdef _WIN32
	SetConsoleCtrlHandler(CtrlCHandler, TRUE);
#endif

	// Configure attack
	if (!app.configureAttack(config)) {
		std::cerr << "Error: Failed to configure attack\n";
		return 1;
	}

	// Start attack
	std::cout << "Starting ARP spoofing attack...\n";
	std::cout << "Press Ctrl+C to stop\n\n";

	if (!app.startAttack()) {
		std::cerr << "Error: Attack failed\n";
		return 1;
	}

	return 0;
} 