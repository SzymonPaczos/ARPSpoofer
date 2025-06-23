#pragma once

#include <cstdint>

////////////////////////////////////////////////////////////
/// \brief Structure representing Ethernet frame header
///
/// This structure defines the format of Ethernet frame header according
/// to IEEE 802.3 standard. Contains source and destination MAC addresses
/// and higher layer protocol type (EtherType).
///
/// The structure name "EthHeader" comes from:
/// - "Eth" - abbreviation of "Ethernet"
/// - "Header" - denotes frame header
///
/// \see IpHeader, ArpHeader
///
////////////////////////////////////////////////////////////
struct EthHeader {
	uint8_t dest[6];    ///< Destination MAC address (6 bytes)
	uint8_t src[6];     ///< Source MAC address (6 bytes)
	uint16_t ethertype; ///< Protocol type (0x0800 for IP, 0x0806 for ARP)
};

////////////////////////////////////////////////////////////
/// \brief Structure representing IP packet header
///
/// This structure defines the format of IPv4 packet header according
/// to RFC 791. Contains information about IP version, header length,
/// type of service, packet length, fragmentation identifier,
/// flags, TTL, higher layer protocol, checksum,
/// and source and destination IP addresses.
///
/// The structure name "IpHeader" comes from:
/// - "Ip" - denotes Internet Protocol
/// - "Header" - denotes packet header
///
/// \see EthHeader, ArpHeader
///
////////////////////////////////////////////////////////////
struct IpHeader {
	uint8_t version_ihl;    ///< Version (4 bits) + Header length (4 bits)
	uint8_t tos;            ///< Type of service
	uint16_t total_length;  ///< Total packet length
	uint16_t id;            ///< Identification
	uint16_t flags_offset;  ///< Flags (3 bits) + Fragment offset (13 bits)
	uint8_t ttl;            ///< Time to live
	uint8_t protocol;       ///< Higher layer protocol
	uint16_t checksum;      ///< Header checksum
	uint32_t src_ip;        ///< Source IP address
	uint32_t dest_ip;       ///< Destination IP address
};

////////////////////////////////////////////////////////////
/// \brief Structure representing ARP packet header
///
/// This structure defines the format of ARP (Address Resolution Protocol)
/// packet header according to RFC 826. Contains hardware type,
/// protocol type, hardware and protocol address sizes,
/// operation code, and source/target MAC and IP addresses.
///
/// The structure name "ArpHeader" comes from:
/// - "Arp" - denotes Address Resolution Protocol
/// - "Header" - denotes packet header
///
/// \see EthHeader, IpHeader
///
////////////////////////////////////////////////////////////
struct ArpHeader {
	uint16_t hw_type;       ///< Hardware type (1 for Ethernet)
	uint16_t proto_type;    ///< Protocol type (0x0800 for IP)
	uint8_t hw_size;        ///< Hardware address size (6 for MAC)
	uint8_t proto_size;     ///< Protocol address size (4 for IP)
	uint16_t opcode;        ///< Operation code (1=request, 2=reply)
	uint8_t sender_mac[6];  ///< Sender MAC address
	uint32_t sender_ip;     ///< Sender IP address
	uint8_t target_mac[6];  ///< Target MAC address
	uint32_t target_ip;     ///< Target IP address
};

// Network constants
const uint16_t ETHERTYPE_IP = 0x0800;   ///< Ethernet type for IP
const uint16_t ETHERTYPE_ARP = 0x0806;  ///< Ethernet type for ARP

const uint8_t IP_PROTO_ICMP = 1;        ///< IP protocol for ICMP
const uint8_t IP_PROTO_TCP = 6;         ///< IP protocol for TCP
const uint8_t IP_PROTO_UDP = 17;        ///< IP protocol for UDP

const uint16_t ARP_OP_REQUEST = 1;      ///< ARP operation: request
const uint16_t ARP_OP_REPLY = 2;        ///< ARP operation: reply

const uint16_t HW_TYPE_ETHERNET = 1;    ///< Hardware type: Ethernet 