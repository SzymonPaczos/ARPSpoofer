#pragma once

#include "PlatformAbstraction.hpp"

#ifdef _WIN32

////////////////////////////////////////////////////////////
/// \brief Windows implementation of NetworkInterface
///
/// This class implements the NetworkInterface interface
/// using Windows API (GetAdaptersAddresses, ResolveIpNetEntry2).
/// Provides access to network interface information
/// and MAC address resolution on Windows platform.
///
/// The class name "WindowsNetworkInterface" comes from:
/// - "Windows" - denotes Windows platform
/// - "Network" - denotes network operations
/// - "Interface" - denotes interface implementation
///
/// \see NetworkInterface, PlatformFactory
///
////////////////////////////////////////////////////////////
class WindowsNetworkInterface : public NetworkInterface {
public:
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Initializes WindowsNetworkInterface object.
	///
	////////////////////////////////////////////////////////////
	WindowsNetworkInterface() = default;

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// Releases Windows API resources.
	///
	////////////////////////////////////////////////////////////
	~WindowsNetworkInterface() override = default;

	////////////////////////////////////////////////////////////
	/// \brief Gets list of all network interfaces
	///
	/// Windows implementation using GetAdaptersAddresses()
	/// to retrieve information about all network adapters.
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
	/// Windows implementation using ResolveIpNetEntry2()
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
	/// \brief Converts Unicode string to string
	///
	/// Helper function to convert interface names
	/// from Unicode format to standard string.
	///
	/// \param unicode Unicode string to convert
	///
	/// \return std::string Converted string
	///
	////////////////////////////////////////////////////////////
	std::string unicodeToString(wchar_t* unicode);
};

////////////////////////////////////////////////////////////
/// \brief Windows implementation of RawSocket
///
/// This class implements the RawSocket interface
/// using Windows Winsock API. Provides raw socket
/// functionality for sending and receiving network
/// packets at the network layer level.
///
/// The class name "WindowsRawSocket" comes from:
/// - "Windows" - denotes Windows platform
/// - "Raw" - denotes raw/unprocessed packets
/// - "Socket" - denotes network socket
///
/// \see RawSocket, PlatformFactory
///
////////////////////////////////////////////////////////////
class WindowsRawSocket : public RawSocket {
public:
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Initializes WindowsRawSocket object with default values.
	///
	////////////////////////////////////////////////////////////
	WindowsRawSocket();

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// Automatically closes socket and releases Winsock resources.
	///
	/// \see close()
	///
	////////////////////////////////////////////////////////////
	~WindowsRawSocket() override;

	////////////////////////////////////////////////////////////
	/// \brief Opens raw socket
	///
	/// Windows implementation using socket() and setsockopt()
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
	/// Windows implementation using closesocket()
	/// and WSACleanup() to safely close connection.
	///
	/// \see RawSocket::close()
	///
	////////////////////////////////////////////////////////////
	void close() override;

	////////////////////////////////////////////////////////////
	/// \brief Sends packet through raw socket
	///
	/// Windows implementation using sendto()
	/// to send data through raw socket.
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
	/// Windows implementation using recvfrom()
	/// to receive data from raw socket.
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
	SOCKET socket;    ///< Windows socket handle
	bool isOpen;      ///< Whether socket is open
};

#endif // _WIN32 