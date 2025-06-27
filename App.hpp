#pragma once

#include "PlatformAbstraction.hpp"
#include "IPAddress.hpp"
#include "NetworkHeaders.hpp"
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <functional>
#include <utility>

////////////////////////////////////////////////////////////
/// \brief Main application class for ARP spoofing
///
/// This class is the central point of the application, responsible
/// for coordinating all ARP spoofing operations. It implements
/// the Singleton pattern, ensuring that there is only one
/// application instance in the entire program.
///
/// The class name "App" comes from:
/// - "App" - abbreviation of "Application"
///
/// The class uses modern C++ features such as:
/// - std::unique_ptr for resource management
/// - std::atomic for thread-safe communication
/// - std::function for callbacks
/// - RAII for automatic resource management
///
/// \see NetworkInterface, RawSocket, PlatformFactory, IPAddress
///
////////////////////////////////////////////////////////////
class App {
public:
	////////////////////////////////////////////////////////////
	/// \brief Attack configuration structure
	///
	/// This structure contains all parameters needed
	/// to perform an ARP spoofing attack.
	///
	/// \see configureAttack()
	///
	////////////////////////////////////////////////////////////
	struct AttackConfig {
		IPAddress victimIp;         ///< Victim's IP address
		IPAddress targetIp;         ///< Target's IP address (optional)
		std::string interfaceName;  ///< Interface name (optional)
		bool oneWayMode;            ///< One-way mode flag
		bool dropMode;              ///< Drop packets instead of forwarding
		int arpInterval;            ///< ARP packet interval (seconds)
	};

	////////////////////////////////////////////////////////////
	/// \brief Attack information structure
	///
	/// This structure contains information about the currently
	/// running attack.
	///
	/// \see getAttackInfo()
	///
	////////////////////////////////////////////////////////////
	struct AttackInfo {
		IPAddress victimIp;         ///< Victim's IP address
		IPAddress targetIp;         ///< Target's IP address
		std::vector<uint8_t> victimMac;   ///< Victim's MAC address
		std::vector<uint8_t> targetMac;   ///< Target's MAC address
		std::vector<uint8_t> myMac;       ///< Our MAC address
		std::string interfaceName;        ///< Interface name
		bool isActive;                    ///< Whether attack is active
		uint64_t packetsSent;             ///< Number of sent packets
		uint64_t packetsReceived;         ///< Number of received packets
		uint64_t packetsDropped;          ///< Number of dropped packets
	};

	////////////////////////////////////////////////////////////
	/// \brief Logging callback type
	///
	/// Definition of callback function type used for logging
	/// events in the application.
	///
	/// \param level Log level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
	/// \param message Message to log
	///
	////////////////////////////////////////////////////////////
	using LogCallback = std::function<void(int level, const std::string& message)>;

	////////////////////////////////////////////////////////////
	/// \brief Stop signal callback type
	///
	/// Definition of callback function type used for handling
	/// attack stop signals.
	///
	////////////////////////////////////////////////////////////
	using StopCallback = std::function<void()>;

private:
	std::unique_ptr<NetworkInterface> networkInterface; ///< Network interface
	std::unique_ptr<RawSocket> rawSocket;               ///< Raw socket
	std::atomic<bool> stopFlag;                         ///< Stop flag
	std::atomic<bool> isRunning;                        ///< Whether application is running
	
	AttackConfig config;                                ///< Attack configuration
	AttackInfo attackInfo;                              ///< Attack information
	
	LogCallback logCallback;                            ///< Logging callback
	StopCallback stopCallback;                          ///< Stop callback

	// Singleton pattern
	static std::unique_ptr<App> instance;               ///< Singleton instance

public:
	////////////////////////////////////////////////////////////
	/// \brief Constructor
	///
	/// Private constructor for Singleton pattern.
	/// Initializes the application with default values.
	///
	////////////////////////////////////////////////////////////
	App();

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// Automatically stops the attack and releases resources
	/// when the App object is destroyed.
	///
	/// \see stopAttack()
	///
	////////////////////////////////////////////////////////////
	~App();

	////////////////////////////////////////////////////////////
	/// \brief Gets the singleton instance
	///
	/// This function returns a reference to the single instance
	/// of the App class (Singleton pattern).
	///
	/// \return App& Reference to the application instance
	///
	/// \see ~App()
	///
	////////////////////////////////////////////////////////////
	static App& getInstance();

	////////////////////////////////////////////////////////////
	/// \brief Displays list of available network interfaces
	///
	/// This function retrieves and displays all active
	/// network interfaces available in the system.
	///
	/// \return bool true if operation succeeded
	///
	/// \see NetworkInterface::getInterfaces()
	///
	////////////////////////////////////////////////////////////
	bool listInterfaces();

	////////////////////////////////////////////////////////////
	/// \brief Configures ARP spoofing attack parameters
	///
	/// This function sets all parameters needed to perform
	/// an attack: victim and target IP addresses, network
	/// interface, and operation mode.
	///
	/// \param config Attack configuration
	///
	/// \return bool true if configuration succeeded
	///
	/// \see AttackConfig, startAttack()
	///
	////////////////////////////////////////////////////////////
	bool configureAttack(const AttackConfig& config);

	////////////////////////////////////////////////////////////
	/// \brief Starts the ARP spoofing attack
	///
	/// This function starts the main attack loop, which:
	/// 1. Sends fake ARP packets at specified intervals
	/// 2. Intercepts and forwards network traffic
	/// 3. Handles interrupt signals
	///
	/// The function runs in a loop until stopAttack() is called
	/// or the stop flag is set.
	///
	/// \return bool true if attack ended successfully
	///
	/// \see stopAttack(), configureAttack()
	///
	////////////////////////////////////////////////////////////
	bool startAttack();

	////////////////////////////////////////////////////////////
	/// \brief Stops the ARP spoofing attack
	///
	/// This function safely stops the attack by sending
	/// correct ARP packets to restore normal network
	/// operation.
	///
	/// \see startAttack(), configureAttack()
	///
	////////////////////////////////////////////////////////////
	void stopAttack();

	////////////////////////////////////////////////////////////
	/// \brief Sets the attack stop flag
	///
	/// This function is called by signal handler
	/// to safely stop the main attack loop.
	///
	/// \see startAttack(), stopAttack()
	///
	////////////////////////////////////////////////////////////
	void requestStop() { stopFlag = true; }

	////////////////////////////////////////////////////////////
	/// \brief Checks if attack is currently active
	///
	/// \return bool true if attack is running
	///
	/// \see startAttack(), stopAttack()
	///
	////////////////////////////////////////////////////////////
	bool isAttackActive() const { return isRunning; }

	////////////////////////////////////////////////////////////
	/// \brief Sets the logging callback function
	///
	/// \param callback Function to call for logging
	///
	/// \see LogCallback
	///
	////////////////////////////////////////////////////////////
	void setLogCallback(LogCallback callback) { logCallback = std::move(callback); }

	////////////////////////////////////////////////////////////
	/// \brief Sets the stop signal callback function
	///
	/// \param callback Function to call when attack stops
	///
	/// \see StopCallback
	///
	////////////////////////////////////////////////////////////
	void setStopCallback(StopCallback callback) { stopCallback = std::move(callback); }

	////////////////////////////////////////////////////////////
	/// \brief Gets current attack information
	///
	/// \return const AttackInfo& Reference to attack information
	///
	/// \see AttackInfo
	///
	////////////////////////////////////////////////////////////
	const AttackInfo& getAttackInfo() const { return attackInfo; }

private:
	////////////////////////////////////////////////////////////
	/// \brief Creates ARP spoofing packet
	///
	/// \param victimIp Victim's IP address
	/// \param targetIp Target's IP address
	/// \param victimMac Victim's MAC address
	/// \param targetMac Target's MAC address
	/// \param myMac Our MAC address
	///
	/// \return std::vector<uint8_t> ARP packet data
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> createArpPacket(
		const IPAddress& victimIp,
		const std::vector<uint8_t>& victimMac,
		const IPAddress& spoofedIp,
		const std::vector<uint8_t>& myMac
	);

	////////////////////////////////////////////////////////////
	/// \brief Handles received network packet
	///
	/// \param data Packet data
	///
	////////////////////////////////////////////////////////////
	void handlePacket(const std::vector<uint8_t>& data);

	////////////////////////////////////////////////////////////
	/// \brief Logs a message using the callback
	///
	/// \param level Log level
	/// \param message Message to log
	///
	/// \see LogCallback
	///
	////////////////////////////////////////////////////////////
	void log(int level, const std::string& message);
}; 