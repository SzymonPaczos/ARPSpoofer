#include "InterfaceInfo.hpp"
#include <cstring>

InterfaceInfo::InterfaceInfo() {
	ifIndex = 0;
	name = "";
	description = "";
	memset(mac, 0, 6);
	memset(ip, 0, 4);
	prefixlen = 0;
	memset(gateway, 0, 4);
}

bool InterfaceInfo::isValid() const {
	// Sprawdź czy interfejs ma ustawiony adres IP (nie jest 0.0.0.0)
	if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
		return false;
	}
	
	// Sprawdź czy interfejs ma ustawiony adres MAC (nie jest 00:00:00:00:00:00)
	if (mac[0] == 0 && mac[1] == 0 && mac[2] == 0 && mac[3] == 0 && mac[4] == 0 && mac[5] == 0) {
		return false;
	}
	
	// Sprawdź czy nazwa interfejsu nie jest pusta
	if (name.empty()) {
		return false;
	}
	
	return true;
}

bool InterfaceInfo::isInSameNetwork(const uint8_t ip[4]) const {
	// Oblicz maskę sieci na podstawie prefixlen
	uint32_t mask = ~((1 << (32 - prefixlen)) - 1);
	
	// Oblicz adresy sieci
	uint32_t interface_network = ((uint32_t)this->ip[0] << 24) | 
	                            ((uint32_t)this->ip[1] << 16) | 
	                            ((uint32_t)this->ip[2] << 8) | 
	                            this->ip[3];
	uint32_t target_network = ((uint32_t)ip[0] << 24) | 
	                         ((uint32_t)ip[1] << 16) | 
	                         ((uint32_t)ip[2] << 8) | 
	                         ip[3];
	
	// Zastosuj maskę
	interface_network &= mask;
	target_network &= mask;
	
	return interface_network == target_network;
} 