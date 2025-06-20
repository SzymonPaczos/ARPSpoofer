#include "RawSocket.hpp"
#include <cstdio>

RawSocket::RawSocket() : sock(INVALID_SOCKET), promiscuous(false) {
}

RawSocket::~RawSocket() {
	close();
}

bool RawSocket::open(const std::string& interface_name, bool promisc) {
	// Inicjalizacja Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed\n");
		return false;
	}

	// Tworzenie raw socket
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "Failed to create raw socket: %d\n", WSAGetLastError());
		return false;
	}

	// Ustawienie opcji promiscuous
	DWORD flag = promisc ? 1 : 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVALL, (char*)&flag, sizeof(flag)) != 0) {
		fprintf(stderr, "Failed to set promiscuous mode: %d\n", WSAGetLastError());
	}

	this->promiscuous = promisc;
	return true;
}

bool RawSocket::sendPacket(const uint8_t* data, size_t len) {
	if (sock == INVALID_SOCKET) return false;
	
	// Dla ARP pakietów musimy użyć sendto z adresem docelowym
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	
	int result = sendto(sock, (char*)data, (int)len, 0, (sockaddr*)&addr, sizeof(addr));
	return result != SOCKET_ERROR;
}

bool RawSocket::receivePacket(uint8_t* buffer, size_t buffer_size, size_t& received_size) {
	if (sock == INVALID_SOCKET) return false;
	
	int result = recv(sock, (char*)buffer, (int)buffer_size, 0);
	if (result == SOCKET_ERROR) {
		return false;
	}
	received_size = result;
	return true;
}

void RawSocket::close() {
	if (sock != INVALID_SOCKET) {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
	WSACleanup();
} 