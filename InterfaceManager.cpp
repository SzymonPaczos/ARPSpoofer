#include "InterfaceManager.hpp"
#include <winsock2.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <cstdio>
#include <cstdlib>

InterfaceManager::InterfaceManager() {
	discoverInterfaces();
}

bool InterfaceManager::discoverInterfaces() {
	ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_GATEWAYS;
	ULONG size = 10 * 1024;
	std::vector<uint8_t> buf(size);
	ULONG res = GetAdaptersAddresses(AF_INET, flags, nullptr, (IP_ADAPTER_ADDRESSES *)&buf[0], &size);
	if (res == ERROR_BUFFER_OVERFLOW) {
		buf.resize(size);
		res = GetAdaptersAddresses(AF_INET, flags, nullptr, (IP_ADAPTER_ADDRESSES *)&buf[0], &size);
	}
	if (res != ERROR_SUCCESS) {
		fprintf(stderr, "Can't get list of adapters: %d\n", res);
		return false;
	}

	interfaces.clear();
	IP_ADAPTER_ADDRESSES *p = (IP_ADAPTER_ADDRESSES *)&buf[0];
	for (; p; p = p->Next) {
		if (p->OperStatus != IfOperStatusUp) {
			continue;
		}
		InterfaceInfo ii;
		ii.ifIndex = p->IfIndex;
		ii.name = std::string("\\Device\\NPF_") + p->AdapterName;
		ii.description = NetworkUtils::unicode_to_str(p->Description) + " (" + NetworkUtils::unicode_to_str(p->FriendlyName) + ")";
		memcpy(ii.mac, p->PhysicalAddress, 6);
		if (p->FirstUnicastAddress) {
			memcpy(ii.ip, &((sockaddr_in *)p->FirstUnicastAddress->Address.lpSockaddr)->sin_addr, 4);
			ii.prefixlen = p->FirstUnicastAddress->OnLinkPrefixLength;
		}
		if (p->FirstGatewayAddress) {
			memcpy(ii.gateway, &((sockaddr_in *)p->FirstGatewayAddress->Address.lpSockaddr)->sin_addr, 4);
		}
		interfaces.push_back(ii);
	}
	return true;
}

void InterfaceManager::printInterfaces() const {
	int i = 1;
	for (const InterfaceInfo& iface : interfaces) {
		printf("%d. %s\t%s\n\t%s/%d gw=%s\n", i, iface.name.c_str(), iface.description.c_str(),
			NetworkUtils::ip_to_str(iface.ip).c_str(), iface.prefixlen, NetworkUtils::ip_to_str(iface.gateway).c_str());
		i++;
	}
}

const InterfaceInfo* InterfaceManager::findInterfaceByIndex(int index) const {
	if (index < 1 || index > (int)interfaces.size()) {
		return nullptr;
	}
	return &interfaces[index - 1];
}

const InterfaceInfo* InterfaceManager::findInterfaceByName(const std::string& name) const {
	for (const InterfaceInfo& iface : interfaces) {
		if (iface.name == name) {
			return &iface;
		}
	}
	return nullptr;
}

const InterfaceInfo* InterfaceManager::findInterfaceByNetwork(const uint8_t ip[4]) const {
	for (const InterfaceInfo& iface : interfaces) {
		if (iface.isInSameNetwork(ip)) {
			return &iface;
		}
	}
	return nullptr;
}

bool InterfaceManager::resolveMacAddress(const InterfaceInfo& iface, const uint8_t ip[4], uint8_t mac[6]) const {
	SOCKADDR_INET srcif;
	srcif.Ipv4.sin_family = AF_INET;
	memcpy(&srcif.Ipv4.sin_addr, iface.ip, 4);

	MIB_IPNET_ROW2 row = { 0 };
	row.InterfaceIndex = iface.ifIndex;
	row.Address.Ipv4.sin_family = AF_INET;
	memcpy(&row.Address.Ipv4.sin_addr, ip, 4);
	
	if (ResolveIpNetEntry2(&row, &srcif) != NO_ERROR) {
		return false;
	}
	if (row.State == NlnsReachable) {
		memcpy(mac, row.PhysicalAddress, 6);
		return true;
	}
	return false;
} 