#include "WindowsPlatform.hpp"

#ifdef _WIN32

#include <winsock2.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <cstring>
#include <cstdio>

std::vector<NetworkInterface::InterfaceInfo> WindowsNetworkInterface::getInterfaces() {
	std::vector<InterfaceInfo> interfaces;
	
	ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | 
	              GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_GATEWAYS;
	ULONG size = 10 * 1024;
	std::vector<uint8_t> buf(size);
	
	ULONG res = GetAdaptersAddresses(AF_INET, flags, nullptr, 
	                                (IP_ADAPTER_ADDRESSES*)&buf[0], &size);
	if (res == ERROR_BUFFER_OVERFLOW) {
		buf.resize(size);
		res = GetAdaptersAddresses(AF_INET, flags, nullptr, 
		                          (IP_ADAPTER_ADDRESSES*)&buf[0], &size);
	}
	
	if (res != ERROR_SUCCESS) {
		return interfaces;
	}

	IP_ADAPTER_ADDRESSES* p = (IP_ADAPTER_ADDRESSES*)&buf[0];
	for (; p; p = p->Next) {
		if (p->OperStatus != IfOperStatusUp) {
			continue;
		}
		
		InterfaceInfo info;
		info.name = std::string("\\Device\\NPF_") + p->AdapterName;
		info.description = unicodeToString(p->Description) + " (" + 
		                   unicodeToString(p->FriendlyName) + ")";
		info.isUp = true;
		
		// Adres MAC
		if (p->PhysicalAddressLength == 6) {
			info.mac.assign(p->PhysicalAddress, p->PhysicalAddress + 6);
		}
		
		// Adres IP i maska
		if (p->FirstUnicastAddress) {
			sockaddr_in* addr = (sockaddr_in*)p->FirstUnicastAddress->Address.lpSockaddr;
			info.ip.assign((uint8_t*)&addr->sin_addr, (uint8_t*)&addr->sin_addr + 4);
			info.prefixLength = p->FirstUnicastAddress->OnLinkPrefixLength;
		}
		
		// Brama domyślna
		if (p->FirstGatewayAddress) {
			sockaddr_in* addr = (sockaddr_in*)p->FirstGatewayAddress->Address.lpSockaddr;
			info.gateway.assign((uint8_t*)&addr->sin_addr, (uint8_t*)&addr->sin_addr + 4);
		}
		
		interfaces.push_back(std::move(info));
	}
	
	return interfaces;
}

std::vector<uint8_t> WindowsNetworkInterface::resolveMacAddress(const std::string& interfaceName, 
                                                               const std::vector<uint8_t>& ip) {
	if (ip.size() != 4) {
		return {};
	}
	
	// Znajdź interfejs po nazwie
	auto interfaces = getInterfaces();
	const InterfaceInfo* targetInterface = nullptr;
	
	for (const auto& iface : interfaces) {
		if (iface.name == interfaceName) {
			targetInterface = &iface;
			break;
		}
	}
	
	if (!targetInterface || targetInterface->ip.empty()) {
		return {};
	}
	
	SOCKADDR_INET srcif;
	srcif.Ipv4.sin_family = AF_INET;
	memcpy(&srcif.Ipv4.sin_addr, targetInterface->ip.data(), 4);

	MIB_IPNET_ROW2 row = { 0 };
	row.InterfaceIndex = 0; // Będzie ustawione przez system
	row.Address.Ipv4.sin_family = AF_INET;
	memcpy(&row.Address.Ipv4.sin_addr, ip.data(), 4);
	
	if (ResolveIpNetEntry2(&row, &srcif) != NO_ERROR) {
		return {};
	}
	
	if (row.State == NlnsReachable) {
		return std::vector<uint8_t>(row.PhysicalAddress, row.PhysicalAddress + 6);
	}
	
	return {};
}

std::string WindowsNetworkInterface::unicodeToString(const wchar_t* unistr) {
	if (!unistr) return "";
	
	char buf[100];
	int res = WideCharToMultiByte(CP_ACP, 0, unistr, wcslen(unistr), buf, 100, NULL, NULL);
	return res > 0 ? std::string(buf, res) : std::string();
}

// WindowsRawSocket implementation
WindowsRawSocket::WindowsRawSocket() : sock(INVALID_SOCKET), initialized(false) {
}

WindowsRawSocket::~WindowsRawSocket() {
	close();
}

bool WindowsRawSocket::open(const std::string& interfaceName, bool promiscuous) {
	// Inicjalizacja Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return false;
	}
	initialized = true;

	// Tworzenie raw socket
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (sock == INVALID_SOCKET) {
		return false;
	}

	// Ustawienie opcji promiscuous
	if (promiscuous) {
		DWORD flag = 1;
		setsockopt((SOCKET)sock, SOL_SOCKET, SO_RCVALL, (char*)&flag, sizeof(flag));
	}

	return true;
}

void WindowsRawSocket::close() {
	if (sock != INVALID_SOCKET) {
		closesocket((SOCKET)sock);
		sock = INVALID_SOCKET;
	}
	if (initialized) {
		WSACleanup();
		initialized = false;
	}
}

bool WindowsRawSocket::sendPacket(const std::vector<uint8_t>& data) {
	if (sock == INVALID_SOCKET) return false;
	
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	
	int result = sendto((SOCKET)sock, (char*)data.data(), (int)data.size(), 0, 
	                    (sockaddr*)&addr, sizeof(addr));
	return result != SOCKET_ERROR;
}

std::vector<uint8_t> WindowsRawSocket::receivePacket() {
	if (sock == INVALID_SOCKET) return {};
	
	std::vector<uint8_t> buffer(65536);
	int result = recv((SOCKET)sock, (char*)buffer.data(), (int)buffer.size(), 0);
	
	if (result == SOCKET_ERROR) {
		return {};
	}
	
	buffer.resize(result);
	return buffer;
}

bool WindowsRawSocket::isOpen() const {
	return sock != INVALID_SOCKET;
}

#endif // _WIN32 