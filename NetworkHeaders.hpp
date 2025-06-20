#pragma once

#include <cstdint>

////////////////////////////////////////////////////////////
/// \brief Struktura reprezentująca nagłówek ramki Ethernet
///
/// Struktura ta definiuje format nagłówka ramki Ethernet zgodnie
/// ze standardem IEEE 802.3. Zawiera adresy MAC źródłowy i docelowy
/// oraz typ protokołu wyższego poziomu (EtherType).
///
/// Nazwa struktury "EthHeader" pochodzi od:
/// - "Eth" - skrót od "Ethernet"
/// - "Header" - oznacza nagłówek ramki
///
/// \see IpHeader, ArpHeader
///
////////////////////////////////////////////////////////////
struct EthHeader {
	uint8_t dest[6];    ///< Adres MAC docelowy (6 bajtów)
	uint8_t src[6];     ///< Adres MAC źródłowy (6 bajtów)
	uint16_t ethertype; ///< Typ protokołu (0x0800 dla IP, 0x0806 dla ARP)
};

////////////////////////////////////////////////////////////
/// \brief Struktura reprezentująca nagłówek pakietu IP
///
/// Struktura ta definiuje format nagłówka pakietu IPv4 zgodnie
/// z RFC 791. Zawiera informacje o wersji IP, długości nagłówka,
/// typie usługi, długości pakietu, identyfikatorze fragmentacji,
/// flagach, TTL, protokole wyższego poziomu, sumie kontrolnej
/// oraz adresach IP źródłowym i docelowym.
///
/// Nazwa struktury "IpHeader" pochodzi od:
/// - "Ip" - oznacza protokół Internet Protocol
/// - "Header" - oznacza nagłówek pakietu
///
/// \see EthHeader, ArpHeader
///
////////////////////////////////////////////////////////////
struct IpHeader {
	uint8_t ihl;        ///< Internet Header Length (długość nagłówka w słowach 32-bitowych)
	uint8_t tos;        ///< Type of Service (typ usługi)
	uint16_t len;       ///< Total Length (całkowita długość pakietu)
	uint16_t frag_id;   ///< Fragment Identification (identyfikator fragmentacji)
	uint8_t frag_offs;  ///< Fragment Offset (przesunięcie fragmentu)
	uint8_t ttl;        ///< Time To Live (czas życia pakietu)
	uint8_t proto;      ///< Protocol (protokół wyższego poziomu)
	uint16_t csum;      ///< Checksum (suma kontrolna nagłówka)
	uint8_t src[4];     ///< Source IP Address (adres IP źródłowy)
	uint8_t dest[4];    ///< Destination IP Address (adres IP docelowy)
};

////////////////////////////////////////////////////////////
/// \brief Struktura reprezentująca nagłówek pakietu ARP
///
/// Struktura ta definiuje format nagłówka pakietu ARP (Address Resolution Protocol)
/// zgodnie z RFC 826. Zawiera informacje o typie sprzętu, typie protokołu,
/// długościach adresów, operacji oraz adresach MAC i IP nadawcy i odbiorcy.
///
/// Nazwa struktury "ArpHeader" pochodzi od:
/// - "Arp" - oznacza protokół Address Resolution Protocol
/// - "Header" - oznacza nagłówek pakietu
///
/// \see EthHeader, IpHeader
///
////////////////////////////////////////////////////////////
struct ArpHeader {
	uint16_t htype;        ///< Hardware Type (typ sprzętu, 1 dla Ethernet)
	uint16_t ptype;        ///< Protocol Type (typ protokołu, 0x0800 dla IP)
	uint8_t hlen;          ///< Hardware Address Length (długość adresu sprzętowego, 6 dla MAC)
	uint8_t plen;          ///< Protocol Address Length (długość adresu protokołu, 4 dla IPv4)
	uint16_t op;           ///< Operation (operacja: 1=request, 2=reply)
	uint8_t sender_mac[6]; ///< Sender Hardware Address (adres MAC nadawcy)
	uint8_t sender_ip[4];  ///< Sender Protocol Address (adres IP nadawcy)
	uint8_t target_mac[6]; ///< Target Hardware Address (adres MAC odbiorcy)
	uint8_t target_ip[4];  ///< Target Protocol Address (adres IP odbiorcy)
}; 