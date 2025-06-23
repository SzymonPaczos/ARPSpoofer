#include "LinuxPlatform.hpp"

#ifdef __linux__

#include <cstring>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

////////////////////////////////////////////////////////////
/// \brief LinuxNetworkInterface implementation
///
////////////////////////////////////////////////////////////

std::vector<NetworkInterface::InterfaceInfo> LinuxNetworkInterface::getInterfaces() {
	std::vector<InterfaceInfo> interfaces;
	
	// Open /proc/net/dev to get interface list
	std::ifstream file("/proc/net/dev");
	if (!file.is_open()) {
		return interfaces;
	}
	
	std::string line;
	// Skip header lines
	std::getline(file, line);
	std::getline(file, line);
	
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string interfaceName;
		
		// Parse interface name (remove colon)
		iss >> interfaceName;
		if (interfaceName.empty() || interfaceName.back() != ':') {
			continue;
		}
		interfaceName.pop_back(); // Remove colon
		
		// Skip loopback and down interfaces
		if (interfaceName == "lo" || interfaceName.find("docker") != std::string::npos) {
			continue;
		}
		
		// Get interface flags
		int flags = getInterfaceFlags(interfaceName);
		if (!(flags & IFF_UP)) {
			continue; // Skip down interfaces
		}
		
		InterfaceInfo info;
		info.name = interfaceName;
		info.description = interfaceName;
		info.isUp = true;
		
		// Get MAC address
		info.mac = getInterfaceMacAddress(interfaceName);
		
		// Get IP address
		info.ip = getInterfaceIpAddress(interfaceName);
		
		// Get netmask
		info.prefixLength = getInterfaceNetmask(interfaceName);
		
		// Get gateway
		info.gateway = getDefaultGateway(interfaceName);
		
		interfaces.push_back(std::move(info));
	}
	
	return interfaces;
}

std::vector<uint8_t> LinuxNetworkInterface::resolveMacAddress(const std::string& interfaceName, 
                                                             const std::vector<uint8_t>& ip) {
	if (ip.size() != 4) {
		return {};
	}
	
	// Try to get MAC from ARP table first
	std::ifstream arpFile("/proc/net/arp");
	if (arpFile.is_open()) {
		std::string line;
		std::getline(arpFile, line); // Skip header
		
		while (std::getline(arpFile, line)) {
			std::istringstream iss(line);
			std::string ipStr, hwType, flags, mac, mask, device;
			
			iss >> ipStr >> hwType >> flags >> mac >> mask >> device;
			
			if (device == interfaceName && mac != "00:00:00:00:00:00") {
				// Convert IP string to bytes
				struct in_addr addr;
				if (inet_pton(AF_INET, ipStr.c_str(), &addr) == 1) {
					std::vector<uint8_t> arpIp(4);
					memcpy(arpIp.data(), &addr.s_addr, 4);
					
					if (arpIp == ip) {
						// Parse MAC address
						std::vector<uint8_t> macBytes;
						std::istringstream macStream(mac);
						std::string byteStr;
						
						while (std::getline(macStream, byteStr, ':')) {
							macBytes.push_back(std::stoi(byteStr, nullptr, 16));
						}
						
						if (macBytes.size() == 6) {
							return macBytes;
						}
					}
				}
			}
		}
	}
	
	// If not found in ARP table, send ARP request
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return {};
	}
	
	// Set interface
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
		::close(sock);
		return {};
	}
	
	// Create ARP request packet
	std::vector<uint8_t> arpPacket(60, 0); // Minimum Ethernet frame size
	
	// Ethernet header
	uint8_t* eth = arpPacket.data();
	// Destination: broadcast
	memset(eth, 0xFF, 6);
	// Source: our MAC (will be filled by kernel)
	memset(eth + 6, 0x00, 6);
	// Type: ARP
	eth[12] = 0x08;
	eth[13] = 0x06;
	
	// ARP header
	uint8_t* arp = eth + 14;
	arp[0] = 0x00; arp[1] = 0x01; // Hardware type: Ethernet
	arp[2] = 0x08; arp[3] = 0x00; // Protocol type: IP
	arp[4] = 0x06; // Hardware size
	arp[5] = 0x04; // Protocol size
	arp[6] = 0x00; arp[7] = 0x01; // Operation: request
	// Sender MAC (will be filled by kernel)
	memset(arp + 8, 0x00, 6);
	// Sender IP (our IP)
	memcpy(arp + 14, ip.data(), 4);
	// Target MAC: unknown
	memset(arp + 18, 0x00, 6);
	// Target IP
	memcpy(arp + 24, ip.data(), 4);
	
	// Send ARP request
	struct sockaddr_ll addr;
	memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ARP);
	addr.sll_ifindex = ifr.ifr_ifindex;
	addr.sll_halen = ETH_ALEN;
	memset(addr.sll_addr, 0xFF, ETH_ALEN); // Broadcast
	
	if (sendto(sock, arpPacket.data(), arpPacket.size(), 0, 
	           (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		::close(sock);
		return {};
	}
	
	// Wait for response (simplified - in real implementation would need timeout)
	::close(sock);
	
	// For now, return empty (would need to implement ARP response parsing)
	return {};
}

int LinuxNetworkInterface::getInterfaceFlags(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return 0;
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	int flags = 0;
	if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
		flags = ifr.ifr_flags;
	}
	
	::close(sock);
	return flags;
}

std::vector<uint8_t> LinuxNetworkInterface::getInterfaceMacAddress(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return {};
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	std::vector<uint8_t> mac(6);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
		memcpy(mac.data(), ifr.ifr_hwaddr.sa_data, 6);
	} else {
		mac.clear();
	}
	
	::close(sock);
	return mac;
}

std::vector<uint8_t> LinuxNetworkInterface::getInterfaceIpAddress(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return {};
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	std::vector<uint8_t> ip(4);
	if (ioctl(sock, SIOCGIFADDR, &ifr) == 0) {
		struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
		memcpy(ip.data(), &addr->sin_addr.s_addr, 4);
	} else {
		ip.clear();
	}
	
	::close(sock);
	return ip;
}

uint8_t LinuxNetworkInterface::getInterfaceNetmask(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return 0;
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	uint8_t prefixLength = 0;
	if (ioctl(sock, SIOCGIFNETMASK, &ifr) == 0) {
		struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_netmask;
		uint32_t netmask = ntohl(addr->sin_addr.s_addr);
		
		// Count leading ones
		while (netmask & 0x80000000) {
			prefixLength++;
			netmask <<= 1;
		}
	}
	
	::close(sock);
	return prefixLength;
}

std::vector<uint8_t> LinuxNetworkInterface::getDefaultGateway(const std::string& interfaceName) {
	// Read routing table from /proc/net/route
	std::ifstream routeFile("/proc/net/route");
	if (!routeFile.is_open()) {
		return {};
	}
	
	std::string line;
	std::getline(routeFile, line); // Skip header
	
	while (std::getline(routeFile, line)) {
		std::istringstream iss(line);
		std::string iface, dest, gateway, flags, refcnt, use, metric, mask, mtu, window, irtt;
		
		iss >> iface >> dest >> gateway >> flags >> refcnt >> use >> metric >> mask >> mtu >> window >> irtt;
		
		if (iface == interfaceName && dest == "00000000") { // Default route
			// Convert hex gateway to bytes
			if (gateway != "00000000") {
				std::vector<uint8_t> gatewayBytes(4);
				for (int i = 0; i < 4; i++) {
					std::string byteStr = gateway.substr(i * 2, 2);
					gatewayBytes[3 - i] = std::stoi(byteStr, nullptr, 16); // Little-endian
				}
				return gatewayBytes;
			}
		}
	}
	
	return {};
}

////////////////////////////////////////////////////////////
/// \brief LinuxRawSocket implementation
///
////////////////////////////////////////////////////////////

LinuxRawSocket::LinuxRawSocket() : socketFd(-1), isOpen(false) {
}

LinuxRawSocket::~LinuxRawSocket() {
	close();
}

bool LinuxRawSocket::open(const std::string& interfaceName, bool promiscuous) {
	this->interfaceName = interfaceName;
	
	// Create raw socket
	socketFd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (socketFd < 0) {
		return false;
	}
	
	// Set non-blocking mode
	int flags = fcntl(socketFd, F_GETFL, 0);
	fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
	
	// Bind to interface
	struct sockaddr_ll addr;
	memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	
	// Get interface index
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(socketFd, SIOCGIFINDEX, &ifr) < 0) {
		::close(socketFd);
		socketFd = -1;
		return false;
	}
	
	addr.sll_ifindex = ifr.ifr_ifindex;
	
	if (bind(socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		::close(socketFd);
		socketFd = -1;
		return false;
	}
	
	// Enable promiscuous mode if requested
	if (promiscuous) {
		if (ioctl(socketFd, SIOCGIFFLAGS, &ifr) < 0) {
			::close(socketFd);
			socketFd = -1;
			return false;
		}
		
		ifr.ifr_flags |= IFF_PROMISC;
		
		if (ioctl(socketFd, SIOCSIFFLAGS, &ifr) < 0) {
			::close(socketFd);
			socketFd = -1;
			return false;
		}
	}
	
	isOpen = true;
	return true;
}

void LinuxRawSocket::close() {
	if (socketFd >= 0) {
		::close(socketFd);
		socketFd = -1;
	}
	isOpen = false;
}

bool LinuxRawSocket::sendPacket(const std::vector<uint8_t>& data) {
	if (!isOpen || socketFd < 0) {
		return false;
	}
	
	// Get interface index
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(socketFd, SIOCGIFINDEX, &ifr) < 0) {
		return false;
	}
	
	// Prepare destination address
	struct sockaddr_ll addr;
	memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = ifr.ifr_ifindex;
	addr.sll_halen = ETH_ALEN;
	
	// Extract destination MAC from packet
	if (data.size() >= 6) {
		memcpy(addr.sll_addr, data.data(), ETH_ALEN);
	}
	
	// Send packet
	ssize_t sent = sendto(socketFd, data.data(), data.size(), 0, 
	                     (struct sockaddr*)&addr, sizeof(addr));
	
	return sent == static_cast<ssize_t>(data.size());
}

std::vector<uint8_t> LinuxRawSocket::receivePacket() {
	if (!isOpen || socketFd < 0) {
		return {};
	}
	
	std::vector<uint8_t> buffer(4096); // Max packet size
	
	ssize_t received = recv(socketFd, buffer.data(), buffer.size(), MSG_DONTWAIT);
	if (received <= 0) {
		return {};
	}
	
	buffer.resize(received);
	return buffer;
}

bool LinuxRawSocket::isOpen() const {
	return isOpen && socketFd >= 0;
}

#endif // __linux__ 