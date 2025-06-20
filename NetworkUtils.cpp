#include "NetworkUtils.hpp"
#include <cstring>
#include <cstdio>

std::string NetworkUtils::unicode_to_str(wchar_t* unistr) {
	char buf[100];
	int res = WideCharToMultiByte(CP_ACP, 0, unistr, wcslen(unistr), buf, 100, NULL, NULL);
	return res > 0 ? std::string(buf, res) : std::string();
}

std::string NetworkUtils::ip_to_str(const uint8_t ip[4]) {
	return std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + std::to_string(ip[2]) + "." + std::to_string(ip[3]);
}

std::string NetworkUtils::mac_to_str(const uint8_t mac[6]) {
	char s[18];
	sprintf_s(s, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return std::string(s, 17);
} 