#include "MacOSPlatform.hpp"

#ifdef __APPLE__

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <sys/sysctl.h>
#include <net/bpf.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

////////////////////////////////////////////////////////////
/// \brief MacOSNetworkInterface implementation
///
////////////////////////////////////////////////////////////

std::vector<NetworkInterface::InterfaceInfo> MacOSNetworkInterface::getInterfaces() {
	std::vector<InterfaceInfo> interfaces;
	
	// Get interface list using sysctl
	int mib[6];
	size_t len;
	char *buf, *next, *lim;
	struct if_msghdr *ifm;
	struct sockaddr_dl *sdl;
	
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0;
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		return interfaces;
	}
	
	if ((buf = (char*)malloc(len)) == NULL) {
		return interfaces;
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		free(buf);
		return interfaces;
	}
	
	lim = buf + len;
	for (next = buf; next < lim; next += ifm->ifm_msglen) {
		ifm = (struct if_msghdr *)next;
		if (ifm->ifm_type == RTM_IFINFO) {
			sdl = (struct sockaddr_dl *)(ifm + 1);
			
			// Skip loopback and down interfaces
			if (!(ifm->ifm_flags & IFF_UP) || (ifm->ifm_flags & IFF_LOOPBACK)) {
				continue;
			}
			
			// Get interface name
			std::string interfaceName(sdl->sdl_data, sdl->sdl_nlen);
			
			// Skip virtual interfaces
			if (interfaceName.find("vmnet") != std::string::npos || 
				interfaceName.find("utun") != std::string::npos) {
				continue;
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
	}
	
	free(buf);
	return interfaces;
}

std::vector<uint8_t> MacOSNetworkInterface::resolveMacAddress(const std::string& interfaceName, 
                                                             const std::vector<uint8_t>& ip) {
	if (ip.size() != 4) {
		return {};
	}
	
	// Try to get MAC from ARP table using sysctl
	int mib[6];
	size_t len;
	char *buf, *next, *lim;
	struct rt_msghdr *rtm;
	struct sockaddr *sa;
	
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_FLAGS;
	mib[5] = RTF_LLINFO;
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		return {};
	}
	
	if ((buf = (char*)malloc(len)) == NULL) {
		return {};
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		free(buf);
		return {};
	}
	
	lim = buf + len;
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		if (rtm->rtm_type == RTM_GET) {
			sa = (struct sockaddr *)(rtm + 1);
			
			// Check if this is an ARP entry
			if (sa->sa_family == AF_INET) {
				struct sockaddr_in *sin = (struct sockaddr_in *)sa;
				
				// Check if this is the IP we're looking for
				std::vector<uint8_t> arpIp(4);
				memcpy(arpIp.data(), &sin->sin_addr.s_addr, 4);
				
				if (arpIp == ip) {
					// Get the link layer address
					sa = (struct sockaddr *)((char *)sa + sizeof(struct sockaddr_in));
					if (sa->sa_family == AF_LINK) {
						struct sockaddr_dl *sdl = (struct sockaddr_dl *)sa;
						if (sdl->sdl_alen == 6) {
							std::vector<uint8_t> mac(6);
							memcpy(mac.data(), LLADDR(sdl), 6);
							free(buf);
							return mac;
						}
					}
				}
			}
		}
	}
	
	free(buf);
	
	// If not found in ARP table, try to ping the device to populate ARP table
	// This is a simple workaround - send a ping to force ARP resolution
	std::string ipStr = std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + 
	                   std::to_string(ip[2]) + "." + std::to_string(ip[3]);
	
	std::string pingCmd = "ping -c 1 -W 1000 " + ipStr + " > /dev/null 2>&1";
	if (system(pingCmd.c_str()) == 0) {
		// Ping succeeded, try to get MAC from ARP table again
		if (sysctl(mib, 6, NULL, &len, NULL, 0) >= 0) {
			if ((buf = (char*)malloc(len)) != NULL) {
				if (sysctl(mib, 6, buf, &len, NULL, 0) >= 0) {
					lim = buf + len;
					for (next = buf; next < lim; next += rtm->rtm_msglen) {
						rtm = (struct rt_msghdr *)next;
						if (rtm->rtm_type == RTM_GET) {
							sa = (struct sockaddr *)(rtm + 1);
							
							if (sa->sa_family == AF_INET) {
								struct sockaddr_in *sin = (struct sockaddr_in *)sa;
								
								std::vector<uint8_t> arpIp(4);
								memcpy(arpIp.data(), &sin->sin_addr.s_addr, 4);
								
								if (arpIp == ip) {
									sa = (struct sockaddr *)((char *)sa + sizeof(struct sockaddr_in));
									if (sa->sa_family == AF_LINK) {
										struct sockaddr_dl *sdl = (struct sockaddr_dl *)sa;
										if (sdl->sdl_alen == 6) {
											std::vector<uint8_t> mac(6);
											memcpy(mac.data(), LLADDR(sdl), 6);
											free(buf);
											return mac;
										}
									}
								}
							}
						}
					}
				}
				free(buf);
			}
		}
	}
	
	// If still not found, try using arp command
	std::string arpCmd = "arp -n " + ipStr + " | grep " + ipStr;
	FILE* pipe = popen(arpCmd.c_str(), "r");
	if (pipe) {
		char buffer[128];
		std::string result = "";
		while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
			result += buffer;
		}
		pclose(pipe);
		
		// Parse MAC address from arp output
		// Format: IP address at MAC address on interface
		size_t atPos = result.find(" at ");
		if (atPos != std::string::npos) {
			size_t onPos = result.find(" on ", atPos);
			if (onPos != std::string::npos) {
				std::string macStr = result.substr(atPos + 4, onPos - atPos - 4);
				// Parse MAC address (format: xx:xx:xx:xx:xx:xx)
				std::vector<uint8_t> mac;
				std::istringstream macStream(macStr);
				std::string byteStr;
				
				while (std::getline(macStream, byteStr, ':')) {
					if (byteStr.length() == 2) {
						mac.push_back(std::stoi(byteStr, nullptr, 16));
					}
				}
				
				if (mac.size() == 6) {
					return mac;
				}
			}
		}
	}
	
	return {};
}

int MacOSNetworkInterface::getInterfaceFlags(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return 0;
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
		::close(sock);
		return 0;
	}
	
	::close(sock);
	return ifr.ifr_flags;
}

std::vector<uint8_t> MacOSNetworkInterface::getInterfaceMacAddress(const std::string& interfaceName) {
	// Use sysctl to get interface MAC address on macOS
	int mib[6];
	size_t len;
	char *buf, *next, *lim;
	struct if_msghdr *ifm;
	struct sockaddr_dl *sdl;
	
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0;
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		return {};
	}
	
	if ((buf = (char*)malloc(len)) == NULL) {
		return {};
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		free(buf);
		return {};
	}
	
	lim = buf + len;
	for (next = buf; next < lim; next += ifm->ifm_msglen) {
		ifm = (struct if_msghdr *)next;
		if (ifm->ifm_type == RTM_IFINFO) {
			sdl = (struct sockaddr_dl *)(ifm + 1);
			
			// Check if this is the interface we're looking for
			std::string currentInterface(sdl->sdl_data, sdl->sdl_nlen);
			if (currentInterface == interfaceName && sdl->sdl_alen == 6) {
				std::vector<uint8_t> mac(6);
				memcpy(mac.data(), LLADDR(sdl), 6);
				free(buf);
				return mac;
			}
		}
	}
	
	free(buf);
	return {};
}

std::vector<uint8_t> MacOSNetworkInterface::getInterfaceIpAddress(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return {};
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
		::close(sock);
		return {};
	}
	
	::close(sock);
	
	struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
	std::vector<uint8_t> ip(4);
	memcpy(ip.data(), &addr->sin_addr.s_addr, 4);
	return ip;
}

uint8_t MacOSNetworkInterface::getInterfaceNetmask(const std::string& interfaceName) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return 24; // Default to /24
	}
	
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(sock, SIOCGIFNETMASK, &ifr) < 0) {
		::close(sock);
		return 24; // Default to /24
	}
	
	::close(sock);
	
	struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
	uint32_t netmask = ntohl(addr->sin_addr.s_addr);
	
	// Convert netmask to prefix length
	uint8_t prefixLength = 0;
	while (netmask & 0x80000000) {
		prefixLength++;
		netmask <<= 1;
	}
	
	return prefixLength;
}

std::vector<uint8_t> MacOSNetworkInterface::getDefaultGateway(const std::string& interfaceName) {
	// Get default gateway using sysctl
	int mib[6];
	size_t len;
	char *buf, *next, *lim;
	struct rt_msghdr *rtm;
	struct sockaddr *sa;
	
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_DUMP;
	mib[5] = 0;
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		return {};
	}
	
	if ((buf = (char*)malloc(len)) == NULL) {
		return {};
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		free(buf);
		return {};
	}
	
	lim = buf + len;
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		if (rtm->rtm_type == RTM_GET) {
			sa = (struct sockaddr *)(rtm + 1);
			
			// Look for default route (destination 0.0.0.0)
			if (sa->sa_family == AF_INET) {
				struct sockaddr_in *sin = (struct sockaddr_in *)sa;
				if (sin->sin_addr.s_addr == 0) {
					// Found default route, get gateway
					sa = (struct sockaddr *)((char *)sa + sizeof(struct sockaddr_in));
					if (sa->sa_family == AF_INET) {
						struct sockaddr_in *gateway = (struct sockaddr_in *)sa;
						std::vector<uint8_t> gatewayIp(4);
						memcpy(gatewayIp.data(), &gateway->sin_addr.s_addr, 4);
						free(buf);
						return gatewayIp;
					}
				}
			}
		}
	}
	
	free(buf);
	return {};
}

////////////////////////////////////////////////////////////
/// \brief MacOSRawSocket implementation
///
////////////////////////////////////////////////////////////

MacOSRawSocket::MacOSRawSocket() : bpfFd(-1) {
}

MacOSRawSocket::~MacOSRawSocket() {
	close();
}

bool MacOSRawSocket::open(const std::string& interfaceName, bool promiscuous) {
	// Find available BPF device
	char device[32];
	for (int i = 0; i < 10; i++) {
		snprintf(device, sizeof(device), "/dev/bpf%d", i);
		bpfFd = ::open(device, O_RDWR);
		if (bpfFd >= 0) {
			break;
		}
	}
	
	if (bpfFd < 0) {
		return false;
	}
	
	// Set interface
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	
	if (ioctl(bpfFd, BIOCSETIF, &ifr) < 0) {
		::close(bpfFd);
		bpfFd = -1;
		return false;
	}
	
	// Set promiscuous mode if requested
	if (promiscuous) {
		u_int enable = 1;
		if (ioctl(bpfFd, BIOCPROMISC, &enable) < 0) {
			::close(bpfFd);
			bpfFd = -1;
			return false;
		}
	}
	
	// Set immediate mode
	u_int immediate = 1;
	if (ioctl(bpfFd, BIOCIMMEDIATE, &immediate) < 0) {
		::close(bpfFd);
		bpfFd = -1;
		return false;
	}
	
	// Set buffer size
	u_int bufferSize = 65536;
	if (ioctl(bpfFd, BIOCSBLEN, &bufferSize) < 0) {
		::close(bpfFd);
		bpfFd = -1;
		return false;
	}
	
	this->interfaceName = interfaceName;
	return true;
}

void MacOSRawSocket::close() {
	if (bpfFd >= 0) {
		::close(bpfFd);
		bpfFd = -1;
	}
}

bool MacOSRawSocket::sendPacket(const std::vector<uint8_t>& data) {
	if (bpfFd < 0) {
		return false;
	}
	
	ssize_t written = write(bpfFd, data.data(), data.size());
	return written == static_cast<ssize_t>(data.size());
}

std::vector<uint8_t> MacOSRawSocket::receivePacket() {
	if (bpfFd < 0) {
		return {};
	}
	
	// BPF header structure
	struct bpf_hdr {
		u_int32_t bh_tstamp[2];
		u_int32_t bh_caplen;
		u_int32_t bh_datalen;
		u_int16_t bh_hdrlen;
	};
	
	char buffer[65536];
	ssize_t bytesRead = read(bpfFd, buffer, sizeof(buffer));
	
	if (bytesRead <= 0) {
		return {};
	}
	
	// Parse BPF header
	struct bpf_hdr* header = (struct bpf_hdr*)buffer;
	if (bytesRead < static_cast<ssize_t>(header->bh_hdrlen + header->bh_caplen)) {
		return {};
	}
	
	// Extract packet data (skip BPF header)
	std::vector<uint8_t> packet(header->bh_caplen);
	memcpy(packet.data(), buffer + header->bh_hdrlen, header->bh_caplen);
	
	return packet;
}

bool MacOSRawSocket::isOpen() const {
	return bpfFd >= 0;
}

#endif // __APPLE__ 