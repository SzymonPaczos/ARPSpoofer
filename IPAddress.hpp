#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

////////////////////////////////////////////////////////////
/// \brief Class representing IPv4 address
///
/// This class encapsulates an IPv4 address and provides convenient
/// operations on IP addresses in SFML style. Contains overloaded
/// operators for easy comparison, conversion and manipulation
/// of IP addresses.
///
/// The class name "IPAddress" comes from:
/// - "IP" - denotes Internet Protocol
/// - "Address" - denotes network address
///
/// The class uses modern C++17 features and provides
/// safe memory management.
///
/// \see MACAddress, NetworkUtils
///
////////////////////////////////////////////////////////////
class IPAddress {
private:
	std::vector<uint8_t> address; ///< IP address as vector of 4 bytes

public:
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Creates empty IP address (0.0.0.0).
	///
	////////////////////////////////////////////////////////////
	IPAddress();

	////////////////////////////////////////////////////////////
	/// \brief Constructor from string
	///
	/// Creates IP address from given string in dotted format.
	///
	/// \param ipString IP address in "192.168.1.1" format
	///
	/// \see fromString()
	///
	////////////////////////////////////////////////////////////
	explicit IPAddress(const std::string& ipString);

	////////////////////////////////////////////////////////////
	/// \brief Constructor from byte array
	///
	/// Creates IP address from array of 4 bytes.
	///
	/// \param bytes Array of 4 bytes representing IP address
	///
	////////////////////////////////////////////////////////////
	explicit IPAddress(const uint8_t bytes[4]);

	////////////////////////////////////////////////////////////
	/// \brief Constructor from byte vector
	///
	/// Creates IP address from byte vector.
	///
	/// \param bytes Byte vector (must have 4 elements)
	///
	////////////////////////////////////////////////////////////
	explicit IPAddress(const std::vector<uint8_t>& bytes);

	////////////////////////////////////////////////////////////
	/// \brief Constructor from individual bytes
	///
	/// Creates IP address from 4 individual bytes.
	///
	/// \param b1 First byte
	/// \param b2 Second byte
	/// \param b3 Third byte
	/// \param b4 Fourth byte
	///
	////////////////////////////////////////////////////////////
	IPAddress(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

	////////////////////////////////////////////////////////////
	/// \brief Copy constructor
	///
	/// \param other IP address to copy
	///
	////////////////////////////////////////////////////////////
	IPAddress(const IPAddress& other) = default;

	////////////////////////////////////////////////////////////
	/// \brief Copy assignment operator
	///
	/// \param other IP address to assign
	///
	/// \return IPAddress& Reference to this object
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator=(const IPAddress& other) = default;

	////////////////////////////////////////////////////////////
	/// \brief Move constructor
	///
	/// \param other IP address to move
	///
	////////////////////////////////////////////////////////////
	IPAddress(IPAddress&& other) noexcept = default;

	////////////////////////////////////////////////////////////
	/// \brief Move assignment operator
	///
	/// \param other IP address to move
	///
	/// \return IPAddress& Reference to this object
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator=(IPAddress&& other) noexcept = default;

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// Automatically releases resources.
	///
	////////////////////////////////////////////////////////////
	~IPAddress() = default;

	////////////////////////////////////////////////////////////
	/// \brief Equality comparison operator
	///
	/// \param other IP address to compare
	///
	/// \return bool true if addresses are identical
	///
	/// \see operator!=()
	///
	////////////////////////////////////////////////////////////
	bool operator==(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Inequality comparison operator
	///
	/// \param other IP address to compare
	///
	/// \return bool true if addresses are different
	///
	/// \see operator==()
	///
	////////////////////////////////////////////////////////////
	bool operator!=(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Less than comparison operator
	///
	/// Compares IP addresses lexicographically.
	///
	/// \param other IP address to compare
	///
	/// \return bool true if this address is less
	///
	/// \see operator>(), operator<=(), operator>=()
	///
	////////////////////////////////////////////////////////////
	bool operator<(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Greater than comparison operator
	///
	/// Compares IP addresses lexicographically.
	///
	/// \param other IP address to compare
	///
	/// \return bool true if this address is greater
	///
	/// \see operator<(), operator<=(), operator>=()
	///
	////////////////////////////////////////////////////////////
	bool operator>(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Less than or equal comparison operator
	///
	/// \param other IP address to compare
	///
	/// \return bool true if this address is less or equal
	///
	/// \see operator<(), operator>(), operator>=()
	///
	////////////////////////////////////////////////////////////
	bool operator<=(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Greater than or equal comparison operator
	///
	/// \param other IP address to compare
	///
	/// \return bool true if this address is greater or equal
	///
	/// \see operator<(), operator>(), operator<=()
	///
	////////////////////////////////////////////////////////////
	bool operator>=(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Bitwise AND operator
	///
	/// Performs bitwise AND operation on IP addresses.
	/// Useful for network mask operations.
	///
	/// \param other IP address to AND with
	///
	/// \return IPAddress Result of bitwise AND
	///
	/// \see operator|(), operator~()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator&(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Bitwise OR operator
	///
	/// Performs bitwise OR operation on IP addresses.
	/// Useful for broadcast address calculations.
	///
	/// \param other IP address to OR with
	///
	/// \return IPAddress Result of bitwise OR
	///
	/// \see operator&(), operator~()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator|(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Bitwise NOT operator
	///
	/// Performs bitwise NOT operation on IP address.
	/// Useful for network mask calculations.
	///
	/// \return IPAddress Result of bitwise NOT
	///
	/// \see operator&(), operator|()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator~() const;

	////////////////////////////////////////////////////////////
	/// \brief Bitwise XOR operator
	///
	/// Performs bitwise XOR operation between two IP addresses.
	///
	/// \param other IP address to XOR with
	///
	/// \return IPAddress Result of XOR operation
	///
	/// \see operator&(), operator|(), operator~()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator^(const IPAddress& other) const;

	////////////////////////////////////////////////////////////
	/// \brief Array access operator (non-const)
	///
	/// Provides access to individual bytes of IP address.
	///
	/// \param index Byte index (0-3)
	///
	/// \return uint8_t& Reference to byte at index
	///
	/// \see operator[] const
	///
	////////////////////////////////////////////////////////////
	uint8_t& operator[](size_t index);

	////////////////////////////////////////////////////////////
	/// \brief Array access operator (const)
	///
	/// Provides read-only access to individual bytes of IP address.
	///
	/// \param index Byte index (0-3)
	///
	/// \return const uint8_t& Reference to byte at index
	///
	/// \see operator[]
	///
	////////////////////////////////////////////////////////////
	const uint8_t& operator[](size_t index) const;

	////////////////////////////////////////////////////////////
	/// \brief Left shift operator
	///
	/// Shifts IP address bits to the left.
	///
	/// \param shift Number of bits to shift
	///
	/// \return IPAddress Shifted IP address
	///
	/// \see operator>>()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator<<(int shift) const;

	////////////////////////////////////////////////////////////
	/// \brief Right shift operator
	///
	/// Shifts IP address bits to the right.
	///
	/// \param shift Number of bits to shift
	///
	/// \return IPAddress Shifted IP address
	///
	/// \see operator<<()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator>>(int shift) const;

	////////////////////////////////////////////////////////////
	/// \brief Addition operator
	///
	/// Adds integer value to IP address.
	///
	/// \param value Value to add
	///
	/// \return IPAddress Result of addition
	///
	/// \see operator-()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator+(uint32_t value) const;

	////////////////////////////////////////////////////////////
	/// \brief Subtraction operator
	///
	/// Subtracts integer value from IP address.
	///
	/// \param value Value to subtract
	///
	/// \return IPAddress Result of subtraction
	///
	/// \see operator+()
	///
	////////////////////////////////////////////////////////////
	IPAddress operator-(uint32_t value) const;

	////////////////////////////////////////////////////////////
	/// \brief Pre-increment operator
	///
	/// Increments the IP address by 1.
	///
	/// \return IPAddress& Reference to this object
	///
	/// \see operator++(int), operator--()
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator++();

	////////////////////////////////////////////////////////////
	/// \brief Post-increment operator
	///
	/// Increments the IP address by 1.
	///
	/// \return IPAddress Copy of this object before increment
	///
	/// \see operator++(), operator--(int)
	///
	////////////////////////////////////////////////////////////
	IPAddress operator++(int);

	////////////////////////////////////////////////////////////
	/// \brief Pre-decrement operator
	///
	/// Decrements the IP address by 1.
	///
	/// \return IPAddress& Reference to this object
	///
	/// \see operator--(int), operator++()
	///
	////////////////////////////////////////////////////////////
	IPAddress& operator--();

	////////////////////////////////////////////////////////////
	/// \brief Post-decrement operator
	///
	/// Decrements the IP address by 1.
	///
	/// \return IPAddress Copy of this object before decrement
	///
	/// \see operator--(), operator++(int)
	///
	////////////////////////////////////////////////////////////
	IPAddress operator--(int);

	////////////////////////////////////////////////////////////
	/// \brief String conversion operator
	///
	/// Converts IP address to string representation.
	///
	/// \return std::string String representation of IP address
	///
	/// \see toString()
	///
	////////////////////////////////////////////////////////////
	operator std::string() const;

	////////////////////////////////////////////////////////////
	/// \brief Vector conversion operator
	///
	/// Converts IP address to byte vector.
	///
	/// \return std::vector<uint8_t> Byte vector representation
	///
	/// \see toBytes()
	///
	////////////////////////////////////////////////////////////
	operator std::vector<uint8_t>() const;

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is valid
	///
	/// Validates that the IP address contains exactly 4 bytes
	/// and all values are within valid range (0-255).
	///
	/// \return bool true if address is valid
	///
	/// \see isEmpty()
	///
	////////////////////////////////////////////////////////////
	bool isValid() const;

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is empty
	///
	/// \return bool true if address is empty (no bytes)
	///
	/// \see isValid()
	///
	////////////////////////////////////////////////////////////
	bool isEmpty() const;

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is localhost
	///
	/// \return bool true if address is 127.0.0.1
	///
	/// \see isPrivate(), isPublic()
	///
	////////////////////////////////////////////////////////////
	bool isLocalhost() const;

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is private
	///
	/// Checks if address belongs to private network ranges:
	/// 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
	///
	/// \return bool true if address is private
	///
	/// \see isLocalhost(), isPublic()
	///
	////////////////////////////////////////////////////////////
	bool isPrivate() const;

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is public
	///
	/// \return bool true if address is public (not private or localhost)
	///
	/// \see isLocalhost(), isPrivate()
	///
	////////////////////////////////////////////////////////////
	bool isPublic() const;

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is in same network
	///
	/// \param other Other IP address to check
	/// \param mask Network mask
	///
	/// \return bool true if addresses are in same network
	///
	////////////////////////////////////////////////////////////
	bool isInSameNetwork(const IPAddress& other, const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Converts IP address to string
	///
	/// \return std::string IP address in "192.168.1.1" format
	///
	/// \see fromString()
	///
	////////////////////////////////////////////////////////////
	std::string toString() const;

	////////////////////////////////////////////////////////////
	/// \brief Converts IP address to byte vector
	///
	/// \return std::vector<uint8_t> Vector of 4 bytes
	///
	/// \see fromBytes()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t> toBytes() const;

	////////////////////////////////////////////////////////////
	/// \brief Converts IP address to 32-bit integer
	///
	/// \return uint32_t 32-bit representation of IP address
	///
	/// \see fromUint32()
	///
	////////////////////////////////////////////////////////////
	uint32_t toUint32() const;

	////////////////////////////////////////////////////////////
	/// \brief Creates IP address from string
	///
	/// Static factory method to create IP address from string.
	///
	/// \param ipString IP address string
	///
	/// \return IPAddress IP address object
	///
	/// \see toString()
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromString(const std::string& ipString);

	////////////////////////////////////////////////////////////
	/// \brief Creates IP address from byte vector
	///
	/// Static factory method to create IP address from byte vector.
	///
	/// \param bytes Byte vector (must have 4 elements)
	///
	/// \return IPAddress IP address object
	///
	/// \see toBytes()
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromBytes(const std::vector<uint8_t>& bytes);

	////////////////////////////////////////////////////////////
	/// \brief Creates IP address from 32-bit integer
	///
	/// Static factory method to create IP address from 32-bit integer.
	///
	/// \param value 32-bit integer value
	///
	/// \return IPAddress IP address object
	///
	/// \see toUint32()
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromUint32(uint32_t value);

	////////////////////////////////////////////////////////////
	/// \brief Creates network mask from prefix length
	///
	/// Creates network mask from CIDR prefix length.
	///
	/// \param prefixLength Prefix length (0-32)
	///
	/// \return IPAddress Network mask
	///
	////////////////////////////////////////////////////////////
	static IPAddress fromPrefixLength(uint8_t prefixLength);

	////////////////////////////////////////////////////////////
	/// \brief Gets network address
	///
	/// Calculates network address using given mask.
	///
	/// \param mask Network mask
	///
	/// \return IPAddress Network address
	///
	/// \see getBroadcastAddress()
	///
	////////////////////////////////////////////////////////////
	IPAddress getNetworkAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Gets broadcast address
	///
	/// Calculates broadcast address using given mask.
	///
	/// \param mask Network mask
	///
	/// \return IPAddress Broadcast address
	///
	/// \see getNetworkAddress()
	///
	////////////////////////////////////////////////////////////
	IPAddress getBroadcastAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Gets first host address
	///
	/// Calculates first usable host address in network.
	///
	/// \param mask Network mask
	///
	/// \return IPAddress First host address
	///
	/// \see getLastHostAddress()
	///
	////////////////////////////////////////////////////////////
	IPAddress getFirstHostAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Gets last host address
	///
	/// Calculates last usable host address in network.
	///
	/// \param mask Network mask
	///
	/// \return IPAddress Last host address
	///
	/// \see getFirstHostAddress()
	///
	////////////////////////////////////////////////////////////
	IPAddress getLastHostAddress(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Gets number of hosts in network
	///
	/// Calculates number of usable host addresses in network.
	///
	/// \param mask Network mask
	///
	/// \return uint32_t Number of hosts
	///
	/// \see getFirstHostAddress(), getLastHostAddress()
	///
	////////////////////////////////////////////////////////////
	uint32_t getHostCount(const IPAddress& mask) const;

	////////////////////////////////////////////////////////////
	/// \brief Gets byte at specified index
	///
	/// Safe access to individual bytes with bounds checking.
	///
	/// \param index Byte index (0-3)
	///
	/// \return uint8_t Byte value at index
	///
	/// \see operator[]
	///
	////////////////////////////////////////////////////////////
	uint8_t at(size_t index) const;

	////////////////////////////////////////////////////////////
	/// \brief Gets size of IP address
	///
	/// \return size_t Always returns 4
	///
	////////////////////////////////////////////////////////////
	size_t size() const { return 4; }

	////////////////////////////////////////////////////////////
	/// \brief Checks if IP address is empty
	///
	/// \return bool true if address has no bytes
	///
	/// \see isEmpty()
	///
	////////////////////////////////////////////////////////////
	bool empty() const { return address.empty(); }

	////////////////////////////////////////////////////////////
	/// \brief Gets iterator to beginning
	///
	/// \return std::vector<uint8_t>::const_iterator Iterator to first byte
	///
	/// \see end()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t>::const_iterator begin() const { return address.begin(); }

	////////////////////////////////////////////////////////////
	/// \brief Gets iterator to end
	///
	/// \return std::vector<uint8_t>::const_iterator Iterator past last byte
	///
	/// \see begin()
	///
	////////////////////////////////////////////////////////////
	std::vector<uint8_t>::const_iterator end() const { return address.end(); }

	// Predefined addresses
	static const IPAddress Any;        ///< 0.0.0.0 - any address
	static const IPAddress Localhost;  ///< 127.0.0.1 - localhost
	static const IPAddress Broadcast;  ///< 255.255.255.255 - broadcast
};

////////////////////////////////////////////////////////////
/// \brief Output operator for IPAddress
///
/// Displays IP address in dotted format.
///
/// \param os Output stream
/// \param ip IP address to display
///
/// \return std::ostream& Reference to stream
///
/// \see operator>>()
///
////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const IPAddress& ip);

////////////////////////////////////////////////////////////
/// \brief Input operator for IPAddress
///
/// Reads IP address from stream in dotted format.
///
/// \param is Input stream
/// \param ip IP address to read
///
/// \return std::istream& Reference to stream
///
/// \see operator<<()
///
////////////////////////////////////////////////////////////
std::istream& operator>>(std::istream& is, IPAddress& ip); 