#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

////////////////////////////////////////////////////////////
/// \brief Abstraction for network operations on different platforms
///
/// This class defines an interface for network operations
/// that can be implemented on different platforms
/// (Windows, Linux, macOS). This enables writing
/// cross-platform code without directly using system APIs.
///
/// The class name "NetworkInterface" comes from:
/// - "Network" - denotes network operations
/// - "Interface" - denotes interface/abstraction
///
/// \see WindowsNetworkInterface, LinuxNetworkInterface
///
////////////////////////////////////////////////////////////
class NetworkInterface {
public:
	////////////////////////////////////////////////////////////
	/// \brief Structure containing network interface information
	///
	/// This structure contains all necessary information
	/// about a network interface that is common to all
	/// platforms.
	///
	/// \see NetworkInterface::getInterfaces()
	///
	////////////////////////////////////////////////////////////
	struct InterfaceInfo {
		std::string name;           ///< Interface name
		std::string description;    ///< Interface description
		std::vector<uint8_t> mac;   ///< MAC address (6 bytes)
		std::vector<uint8_t> ip;    ///< IP address (4 bytes)
		uint8_t prefixLength;       ///< Network prefix length
		std::vector<uint8_t> gateway; ///< Default gateway address
		bool isUp;                  ///< Whether interface is active
	};

	////////////////////////////////////////////////////////////
	/// \brief Virtual destructor
	///
	/// Ensures proper resource cleanup during inheritance.
	///
	////////////////////////////////////////////////////////////
	virtual ~NetworkInterface() = default;

	////////////////////////////////////////////////////////////
	/// \brief Gets list of all network interfaces
	///
	/// This function returns a list of all active network
	/// interfaces available in the system.
	///
	/// \return std::vector<InterfaceInfo> List of network interfaces
	///
	/// \see InterfaceInfo
	///
	////////////////////////////////////////////////////////////
	virtual std::vector<InterfaceInfo> getInterfaces() = 0;

	////////////////////////////////////////////////////////////
	/// \brief Resolves IP address to MAC address
	///
	/// This function attempts to find the MAC address corresponding
	/// to the given IP address in the system's ARP table.
	///
	/// \param interfaceName Interface name to search on
	/// \param ip IP address to resolve
	///
	/// \return std::vector<uint8_t> MAC address (6 bytes) or empty vector if not found
	///
	////////////////////////////////////////////////////////////
	virtual std::vector<uint8_t> resolveMacAddress(const std::string& interfaceName, 
	                                               const std::vector<uint8_t>& ip) = 0;
};

////////////////////////////////////////////////////////////
/// \brief Abstraction for raw socket on different platforms
///
/// This class defines an interface for raw socket operations
/// that can be implemented on different platforms.
/// Enables sending and receiving network packets
/// at the network layer level.
///
/// The class name "RawSocket" comes from:
/// - "Raw" - denotes raw/unprocessed packets
/// - "Socket" - denotes network socket
///
/// \see WindowsRawSocket, LinuxRawSocket
///
////////////////////////////////////////////////////////////
class RawSocket {
public:
	////////////////////////////////////////////////////////////
	/// \brief Virtual destructor
	///
	/// Ensures proper resource cleanup during inheritance.
	///
	////////////////////////////////////////////////////////////
	virtual ~RawSocket() = default;

	////////////////////////////////////////////////////////////
	/// \brief Opens raw socket
	///
	/// This function initializes a raw socket on the specified interface.
	///
	/// \param interfaceName Network interface name
	/// \param promiscuous Whether to enable promiscuous mode
	///
	/// \return bool true if socket was successfully opened
	///
	/// \see close(), sendPacket(), receivePacket()
	///
	////////////////////////////////////////////////////////////
	virtual bool open(const std::string& interfaceName, bool promiscuous = true) = 0;

	////////////////////////////////////////////////////////////
	/// \brief Closes raw socket
	///
	/// This function safely closes the raw socket and releases resources.
	///
	/// \see open()
	///
	////////////////////////////////////////////////////////////
	virtual void close() = 0;

	////////////////////////////////////////////////////////////
	/// \brief Sends packet through raw socket
	///
	/// \param data Data to send
	///
	/// \return bool true if packet was successfully sent
	///
	/// \see receivePacket()
	///
	////////////////////////////////////////////////////////////
	virtual bool sendPacket(const std::vector<uint8_t>& data) = 0;

	////////////////////////////////////////////////////////////
	/// \brief Receives packet from raw socket
	///
	/// \return std::vector<uint8_t> Received data or empty vector if no packets
	///
	/// \see sendPacket()
	///
	////////////////////////////////////////////////////////////
	virtual std::vector<uint8_t> receivePacket() = 0;

	////////////////////////////////////////////////////////////
	/// \brief Checks if socket is open
	///
	/// \return bool true if socket is open
	///
	////////////////////////////////////////////////////////////
	virtual bool isOpen() const = 0;
};

////////////////////////////////////////////////////////////
/// \brief Factory for creating platform-specific implementations
///
/// This class creates appropriate implementations for the current
/// platform. This enables dynamic selection of implementations
/// depending on the operating system.
///
/// The class name "PlatformFactory" comes from:
/// - "Platform" - denotes system platform
/// - "Factory" - denotes factory design pattern
///
/// \see NetworkInterface, RawSocket
///
////////////////////////////////////////////////////////////
class PlatformFactory {
public:
	////////////////////////////////////////////////////////////
	/// \brief Creates NetworkInterface implementation for current platform
	///
	/// \return std::unique_ptr<NetworkInterface> Implementation for current platform
	///
	/// \see NetworkInterface
	///
	////////////////////////////////////////////////////////////
	static std::unique_ptr<NetworkInterface> createNetworkInterface();

	////////////////////////////////////////////////////////////
	/// \brief Creates RawSocket implementation for current platform
	///
	/// \return std::unique_ptr<RawSocket> Implementation for current platform
	///
	/// \see RawSocket
	///
	////////////////////////////////////////////////////////////
	static std::unique_ptr<RawSocket> createRawSocket();
}; 