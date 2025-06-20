#include "IPAddress.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cctype>

// Predefiniowane adresy IP
const IPAddress IPAddress::Any(0, 0, 0, 0);
const IPAddress IPAddress::Localhost(127, 0, 0, 1);
const IPAddress IPAddress::Broadcast(255, 255, 255, 255);

////////////////////////////////////////////////////////////
IPAddress::IPAddress() : address(4, 0) {}

////////////////////////////////////////////////////////////
IPAddress::IPAddress(const std::string& ipString) : address(4, 0) {
	*this = fromString(ipString);
}

////////////////////////////////////////////////////////////
IPAddress::IPAddress(const uint8_t bytes[4]) : address(bytes, bytes + 4) {}

////////////////////////////////////////////////////////////
IPAddress::IPAddress(const std::vector<uint8_t>& bytes) : address(bytes) {
	if (address.size() != 4) {
		address.resize(4, 0);
	}
}

////////////////////////////////////////////////////////////
IPAddress::IPAddress(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) 
	: address{b1, b2, b3, b4} {}

////////////////////////////////////////////////////////////
bool IPAddress::operator==(const IPAddress& other) const {
	return address == other.address;
}

////////////////////////////////////////////////////////////
bool IPAddress::operator!=(const IPAddress& other) const {
	return !(*this == other);
}

////////////////////////////////////////////////////////////
bool IPAddress::operator<(const IPAddress& other) const {
	return address < other.address;
}

////////////////////////////////////////////////////////////
bool IPAddress::operator>(const IPAddress& other) const {
	return other < *this;
}

////////////////////////////////////////////////////////////
bool IPAddress::operator<=(const IPAddress& other) const {
	return !(other < *this);
}

////////////////////////////////////////////////////////////
bool IPAddress::operator>=(const IPAddress& other) const {
	return !(*this < other);
}

////////////////////////////////////////////////////////////
uint8_t& IPAddress::operator[](size_t index) {
	return address[index];
}

////////////////////////////////////////////////////////////
const uint8_t& IPAddress::operator[](size_t index) const {
	return address[index];
}

////////////////////////////////////////////////////////////
IPAddress::operator std::string() const {
	return toString();
}

////////////////////////////////////////////////////////////
IPAddress::operator std::vector<uint8_t>() const {
	return toBytes();
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator&(const IPAddress& other) const {
	IPAddress result;
	for (size_t i = 0; i < 4; ++i) {
		result.address[i] = address[i] & other.address[i];
	}
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator|(const IPAddress& other) const {
	IPAddress result;
	for (size_t i = 0; i < 4; ++i) {
		result.address[i] = address[i] | other.address[i];
	}
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator^(const IPAddress& other) const {
	IPAddress result;
	for (size_t i = 0; i < 4; ++i) {
		result.address[i] = address[i] ^ other.address[i];
	}
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator~() const {
	IPAddress result;
	for (size_t i = 0; i < 4; ++i) {
		result.address[i] = ~address[i];
	}
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator<<(int shift) const {
	IPAddress result = *this;
	uint32_t value = toUint32();
	value <<= shift;
	result = fromUint32(value);
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator>>(int shift) const {
	IPAddress result = *this;
	uint32_t value = toUint32();
	value >>= shift;
	result = fromUint32(value);
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator+(uint32_t value) const {
	uint32_t current = toUint32();
	current += value;
	return fromUint32(current);
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator-(uint32_t value) const {
	uint32_t current = toUint32();
	current -= value;
	return fromUint32(current);
}

////////////////////////////////////////////////////////////
IPAddress& IPAddress::operator++() {
	uint32_t value = toUint32();
	value++;
	*this = fromUint32(value);
	return *this;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator++(int) {
	IPAddress temp = *this;
	++(*this);
	return temp;
}

////////////////////////////////////////////////////////////
IPAddress& IPAddress::operator--() {
	uint32_t value = toUint32();
	value--;
	*this = fromUint32(value);
	return *this;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::operator--(int) {
	IPAddress temp = *this;
	--(*this);
	return temp;
}

////////////////////////////////////////////////////////////
bool IPAddress::isValid() const {
	return !isEmpty();
}

////////////////////////////////////////////////////////////
bool IPAddress::isEmpty() const {
	return *this == Any;
}

////////////////////////////////////////////////////////////
bool IPAddress::isLocalhost() const {
	return address[0] == 127;
}

////////////////////////////////////////////////////////////
bool IPAddress::isPrivate() const {
	// 10.0.0.0/8
	if (address[0] == 10) return true;
	
	// 172.16.0.0/12
	if (address[0] == 172 && address[1] >= 16 && address[1] <= 31) return true;
	
	// 192.168.0.0/16
	if (address[0] == 192 && address[1] == 168) return true;
	
	return false;
}

////////////////////////////////////////////////////////////
bool IPAddress::isPublic() const {
	return !isLocalhost() && !isPrivate() && !isEmpty();
}

////////////////////////////////////////////////////////////
bool IPAddress::isInSameNetwork(const IPAddress& other, const IPAddress& mask) const {
	IPAddress network1 = getNetworkAddress(mask);
	IPAddress network2 = other.getNetworkAddress(mask);
	return network1 == network2;
}

////////////////////////////////////////////////////////////
std::string IPAddress::toString() const {
	std::ostringstream oss;
	oss << static_cast<int>(address[0]) << "."
		<< static_cast<int>(address[1]) << "."
		<< static_cast<int>(address[2]) << "."
		<< static_cast<int>(address[3]);
	return oss.str();
}

////////////////////////////////////////////////////////////
std::vector<uint8_t> IPAddress::toBytes() const {
	return address;
}

////////////////////////////////////////////////////////////
uint32_t IPAddress::toUint32() const {
	return (static_cast<uint32_t>(address[0]) << 24) |
		   (static_cast<uint32_t>(address[1]) << 16) |
		   (static_cast<uint32_t>(address[2]) << 8) |
		   static_cast<uint32_t>(address[3]);
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::fromString(const std::string& ipString) {
	IPAddress result;
	std::istringstream iss(ipString);
	std::string token;
	
	for (int i = 0; i < 4; ++i) {
		if (!std::getline(iss, token, '.')) {
			return Any; // Błąd parsowania
		}
		
		// Sprawdź czy token zawiera tylko cyfry
		if (token.empty() || token.find_first_not_of("0123456789") != std::string::npos) {
			return Any;
		}
		
		int value = std::stoi(token);
		if (value < 0 || value > 255) {
			return Any; // Nieprawidłowa wartość
		}
		
		result.address[i] = static_cast<uint8_t>(value);
	}
	
	// Sprawdź czy nie ma dodatkowych tokenów
	if (std::getline(iss, token)) {
		return Any;
	}
	
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::fromBytes(const std::vector<uint8_t>& bytes) {
	if (bytes.size() != 4) {
		return Any;
	}
	return IPAddress(bytes);
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::fromUint32(uint32_t value) {
	IPAddress result;
	result.address[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
	result.address[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
	result.address[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
	result.address[3] = static_cast<uint8_t>(value & 0xFF);
	return result;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::fromPrefixLength(uint8_t prefixLength) {
	if (prefixLength > 32) {
		return Any;
	}
	
	uint32_t mask = 0xFFFFFFFF << (32 - prefixLength);
	return fromUint32(mask);
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::getNetworkAddress(const IPAddress& mask) const {
	return *this & mask;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::getBroadcastAddress(const IPAddress& mask) const {
	return *this | ~mask;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::getFirstHostAddress(const IPAddress& mask) const {
	IPAddress network = getNetworkAddress(mask);
	return network + 1;
}

////////////////////////////////////////////////////////////
IPAddress IPAddress::getLastHostAddress(const IPAddress& mask) const {
	IPAddress broadcast = getBroadcastAddress(mask);
	return broadcast - 1;
}

////////////////////////////////////////////////////////////
uint32_t IPAddress::getHostCount(const IPAddress& mask) const {
	IPAddress network = getNetworkAddress(mask);
	IPAddress broadcast = network.getBroadcastAddress(mask);
	
	uint32_t networkValue = network.toUint32();
	uint32_t broadcastValue = broadcast.toUint32();
	
	return broadcastValue - networkValue - 1;
}

////////////////////////////////////////////////////////////
uint8_t IPAddress::at(size_t index) const {
	if (index >= 4) {
		throw std::out_of_range("IPAddress index out of range");
	}
	return address[index];
}

////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const IPAddress& ip) {
	os << ip.toString();
	return os;
}

////////////////////////////////////////////////////////////
std::istream& operator>>(std::istream& is, IPAddress& ip) {
	std::string ipString;
	is >> ipString;
	ip = IPAddress::fromString(ipString);
	return is;
} 