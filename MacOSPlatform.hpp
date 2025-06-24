#pragma once

#include "PlatformAbstraction.hpp"

#ifdef __APPLE__

#include <string>
#include <vector>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <net/bpf.h>

////////////////////////////////////////////////////////////
/// \brief macOS implementation of NetworkInterface
///
/// This class implements the NetworkInterface interface
/// using macOS system calls (ioctl, socket, sysctl). Provides access
/// to network interface information and MAC address resolution
/// on macOS platform.
///
/// The class name "MacOSNetworkInterface" comes from:
/// - "MacOS" - denotes macOS platform
/// - "Network" - denotes network operations
/// - "Interface" - denotes interface implementation
///
/// \see NetworkInterface, PlatformFactory
///
////////////////////////////////////////////////////////////
class MacOSNetworkInterface : public NetworkInterface {
public:
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Initializes MacOSNetworkInterface object.
	///
	////////////////////////////////////////////////////////////
	MacOSNetworkInterface() = default;

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// Releases macOS system resources.
	///
	////////////////////////////////////////////////////////////
	~MacOSNetworkInterface() override = default;

	////////////////////////////////////////////////////////////
	/// \brief Gets list of all network interfaces
	///
	/// macOS implementation using sysctl() calls
	/// to retrieve information about all network interfaces.
	///
	/// \return std::vector<InterfaceInfo> List of network interfaces
	///
	/// \see NetworkInterface::getInterfaces()
	///
	////////////////////////////////////////////////////////////
	std::vector<InterfaceInfo> getInterfaces() override;

	////////////////////////////////////////////////////////////
	/// \brief Resolves IP address to MAC address
	///
	/// macOS implementation using ARP table lookup
	/// to search for MAC address in system ARP table.
	///
	/// \param interfaceName Interface name to search on
	/// \param ip IP address to resolve
	///
	/// \return std::vector<uint8_t> MAC address (6 bytes) or empty vector
	///
	/// \see NetworkInterface::resolveMacAddress()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> resolveMacAddress(const std::string& interfaceName, 
	                                       const std::vector<uint8_t>& ip) override;

private:
	////////////////////////////////////////////////////////////
	/// \brief Gets interface flags
	///
	/// Helper function to get interface status flags.
	///
	/// \param interfaceName Interface name
	///
	/// \return int Interface flags
	///
	////////////////////////////////////////////////////////////
	int getInterfaceFlags(const std::string& interfaceName);

	////////////////////////////////////////////////////////////
	/// \brief Gets interface MAC address
	///
	/// Helper function to get interface MAC address.
	///
	/// \param interfaceName Interface name
	///
	/// \return std::vector<uint8_t> MAC address
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> getInterfaceMacAddress(const std::string& interfaceName);

	////////////////////////////////////////////////////////////
	/// \brief Gets interface IP address
	///
	/// Helper function to get interface IP address.
	///
	/// \param interfaceName Interface name
	///
	/// \return std::vector<uint8_t> IP address
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> getInterfaceIpAddress(const std::string& interfaceName);

	////////////////////////////////////////////////////////////
	/// \brief Gets interface netmask
	///
	/// Helper function to get interface netmask.
	///
	/// \param interfaceName Interface name
	///
	/// \return uint8_t Netmask prefix length
	///
	////////////////////////////////////////////////////////////
	uint8_t getInterfaceNetmask(const std::string& interfaceName);

	////////////////////////////////////////////////////////////
	/// \brief Gets default gateway
	///
	/// Helper function to get default gateway address.
	///
	/// \param interfaceName Interface name
	///
	/// \return std::vector<uint8_t> Gateway IP address
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> getDefaultGateway(const std::string& interfaceName);
};

////////////////////////////////////////////////////////////
/// \brief macOS implementation of RawSocket
///
/// This class implements the RawSocket interface
/// using macOS BPF (Berkeley Packet Filter) API. Provides raw socket
/// functionality for sending and receiving network
/// packets at the network layer level.
///
/// The class name "MacOSRawSocket" comes from:
/// - "MacOS" - denotes macOS platform
/// - "Raw" - denotes raw/unprocessed packets
/// - "Socket" - denotes network socket
///
/// \see RawSocket, PlatformFactory
///
////////////////////////////////////////////////////////////
class MacOSRawSocket : public RawSocket {
public:
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Initializes MacOSRawSocket object with default values.
	///
	////////////////////////////////////////////////////////////
	MacOSRawSocket();

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// Automatically closes socket and releases system resources.
	///
	/// \see close()
	///
	////////////////////////////////////////////////////////////
	~MacOSRawSocket() override;

	////////////////////////////////////////////////////////////
	/// \brief Opens raw socket
	///
	/// macOS implementation using BPF device
	/// to create raw socket with optional promiscuous mode.
	///
	/// \param interfaceName Network interface name
	/// \param promiscuous Whether to enable promiscuous mode
	///
	/// \return bool true if socket was successfully opened
	///
	/// \see RawSocket::open()
	///
	////////////////////////////////////////////////////////////
	bool open(const std::string& interfaceName, bool promiscuous = true) override;

	////////////////////////////////////////////////////////////
	/// \brief Closes raw socket
	///
	/// macOS implementation using close()
	/// to safely close connection.
	///
	/// \see RawSocket::close()
	///
	////////////////////////////////////////////////////////////
	void close() override;

	////////////////////////////////////////////////////////////
	/// \brief Sends packet through raw socket
	///
	/// macOS implementation using write()
	/// to send data through BPF device.
	///
	/// \param data Data to send
	///
	/// \return bool true if packet was successfully sent
	///
	/// \see RawSocket::sendPacket()
	///
	////////////////////////////////////////////////////////////
	bool sendPacket(const std::vector<uint8_t>& data) override;

	////////////////////////////////////////////////////////////
	/// \brief Receives packet from raw socket
	///
	/// macOS implementation using read()
	/// to receive data from BPF device.
	///
	/// \return std::vector<uint8_t> Received data or empty vector
	///
	/// \see RawSocket::receivePacket()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> receivePacket() override;

	////////////////////////////////////////////////////////////
	/// \brief Checks if socket is open
	///
	/// \return bool true if socket is open
	///
	/// \see RawSocket::isOpen()
	///
	////////////////////////////////////////////////////////////
	bool isOpen() const override;

private:
	int bpfFd;        ///< macOS BPF file descriptor
	bool isOpen;      ///< Whether socket is open
	std::string interfaceName; ///< Interface name
};

#endif // __APPLE__ 