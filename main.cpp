#include "App.hpp"
#include "IPAddress.hpp"
#include <iostream>
#include <string>
#include <cstring>

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
	std::cout << "Usage: " << programName << " [OPTIONS] <victim-ip> [target-ip]\n\n";
	std::cout << "Options:\n";
	std::cout << "  --help, -h          Display this help\n";
	std::cout << "  --list, -l          Display list of network interfaces\n";
	std::cout << "  --interface, -i     Specify network interface\n";
	std::cout << "  --oneway, -o        One-way attack only\n";
	std::cout << "  --interval, -t      ARP packet interval (seconds, default 2)\n";
	std::cout << "  --verbose, -v       Detailed logging\n\n";
	std::cout << "Arguments:\n";
	std::cout << "  victim-ip           Victim's IP address (required)\n";
	std::cout << "  target-ip           Target's IP address (optional, default gateway)\n\n";
	std::cout << "Examples:\n";
	std::cout << "  " << programName << " --list\n";
	std::cout << "  " << programName << " 192.168.1.10\n";
	std::cout << "  " << programName << " -i eth0 192.168.1.10\n";
	std::cout << "  " << programName << " --oneway 192.168.1.10\n";
	std::cout << "  " << programName << " -t 5 192.168.1.10 192.168.1.1\n\n";
	std::cout << "WARNING: Program requires administrator privileges!\n";
	std::cout << "         Use only in controlled environment.\n";
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
		return 1;
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