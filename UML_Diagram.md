# ARP Spoofing Tool - UML Class Diagram

## Class Diagram

```mermaid
classDiagram
    %% Main Application Class
    class App {
        -NetworkInterface* networkInterface
        -RawSocket* rawSocket
        -atomic<bool> stopFlag
        -atomic<bool> isRunning
        -AttackConfig config
        -AttackInfo attackInfo
        -LogCallback logCallback
        -StopCallback stopCallback
        -static App* instance
        +App()
        +~App()
        +static App& getInstance()
        +bool listInterfaces()
        +bool configureAttack(AttackConfig)
        +bool startAttack()
        +void stopAttack()
        +void requestStop()
        +bool isAttackActive()
        +void setLogCallback(LogCallback)
        +void setStopCallback(StopCallback)
        -void handlePacket(vector<uint8_t>)
        -void log(int, string)
    }

    %% Configuration Structures
    class AttackConfig {
        +IPAddress victimIp
        +IPAddress targetIp
        +string interfaceName
        +bool oneWayMode
        +int arpInterval
    }

    class AttackInfo {
        +IPAddress victimIp
        +IPAddress targetIp
        +vector<uint8_t> victimMac
        +vector<uint8_t> targetMac
        +vector<uint8_t> myMac
        +string interfaceName
        +bool isActive
        +uint64_t packetsSent
        +uint64_t packetsReceived
    }

    %% Platform Abstraction
    class NetworkInterface {
        <<interface>>
        +struct InterfaceInfo
        +virtual ~NetworkInterface()
        +virtual vector<InterfaceInfo> getInterfaces() = 0
        +virtual vector<uint8_t> resolveMacAddress(string, vector<uint8_t>) = 0
    }

    class RawSocket {
        <<interface>>
        +virtual ~RawSocket()
        +virtual bool open(string, bool) = 0
        +virtual void close() = 0
        +virtual bool sendPacket(vector<uint8_t>) = 0
        +virtual vector<uint8_t> receivePacket() = 0
        +virtual bool isOpen() = 0
    }

    class PlatformFactory {
        <<static>>
        +static unique_ptr<NetworkInterface> createNetworkInterface()
        +static unique_ptr<RawSocket> createRawSocket()
    }

    %% Windows Implementation
    class WindowsNetworkInterface {
        +WindowsNetworkInterface()
        +~WindowsNetworkInterface()
        +vector<InterfaceInfo> getInterfaces()
        +vector<uint8_t> resolveMacAddress(string, vector<uint8_t>)
        -string unicodeToString(wchar_t*)
    }

    class WindowsRawSocket {
        -SOCKET socket
        -bool isOpen
        +WindowsRawSocket()
        +~WindowsRawSocket()
        +bool open(string, bool)
        +void close()
        +bool sendPacket(vector<uint8_t>)
        +vector<uint8_t> receivePacket()
        +bool isOpen()
    }

    %% IP Address Class
    class IPAddress {
        -vector<uint8_t> address
        +IPAddress()
        +IPAddress(string)
        +IPAddress(uint8_t[4])
        +IPAddress(vector<uint8_t>)
        +IPAddress(uint8_t, uint8_t, uint8_t, uint8_t)
        +bool operator==(IPAddress)
        +bool operator!=(IPAddress)
        +bool operator<(IPAddress)
        +bool operator>(IPAddress)
        +IPAddress operator&(IPAddress)
        +IPAddress operator|(IPAddress)
        +IPAddress operator~(IPAddress)
        +bool isValid()
        +bool isEmpty()
        +bool isLocalhost()
        +bool isPrivate()
        +bool isPublic()
        +string toString()
        +vector<uint8_t> toBytes()
        +uint32_t toUint32()
        +static IPAddress fromString(string)
        +static IPAddress fromBytes(vector<uint8_t>)
        +static IPAddress fromUint32(uint32_t)
        +static const IPAddress Any
        +static const IPAddress Localhost
        +static const IPAddress Broadcast
    }

    %% Network Headers
    class EthHeader {
        +uint8_t dest[6]
        +uint8_t src[6]
        +uint16_t ethertype
    }

    class IpHeader {
        +uint8_t version_ihl
        +uint8_t tos
        +uint16_t total_length
        +uint16_t id
        +uint16_t flags_offset
        +uint8_t ttl
        +uint8_t protocol
        +uint16_t checksum
        +uint32_t src_ip
        +uint32_t dest_ip
    }

    class ArpHeader {
        +uint16_t hw_type
        +uint16_t proto_type
        +uint8_t hw_size
        +uint8_t proto_size
        +uint16_t opcode
        +uint8_t sender_mac[6]
        +uint32_t sender_ip
        +uint8_t target_mac[6]
        +uint32_t target_ip
    }

    %% Relationships
    App --> NetworkInterface : uses
    App --> RawSocket : uses
    App --> PlatformFactory : uses
    App --> IPAddress : uses
    App --> AttackConfig : contains
    App --> AttackInfo : contains

    NetworkInterface <|-- WindowsNetworkInterface : implements
    RawSocket <|-- WindowsRawSocket : implements

    PlatformFactory --> NetworkInterface : creates
    PlatformFactory --> RawSocket : creates

    NetworkInterface --> InterfaceInfo : contains
    WindowsNetworkInterface --> InterfaceInfo : returns

    %% Network Headers
    App --> EthHeader : uses
    App --> IpHeader : uses
    App --> ArpHeader : uses
```

## Component Diagram

```mermaid
graph TB
    subgraph "ARP Spoofing Application"
        A[App - Main Controller]
        B[PlatformFactory]
        C[NetworkInterface]
        D[RawSocket]
        E[IPAddress]
    end

    subgraph "Windows Platform"
        F[WindowsNetworkInterface]
        G[WindowsRawSocket]
    end

    subgraph "Network Headers"
        H[EthHeader]
        I[IpHeader]
        J[ArpHeader]
    end

    subgraph "Configuration"
        K[AttackConfig]
        L[AttackInfo]
    end

    A --> B
    A --> C
    A --> D
    A --> E
    A --> K
    A --> L

    B --> F
    B --> G

    C --> F
    D --> G

    A --> H
    A --> I
    A --> J
```

## Sequence Diagram - Attack Flow

```mermaid
sequenceDiagram
    participant User
    participant App
    participant PlatformFactory
    participant NetworkInterface
    participant RawSocket
    participant Network

    User->>App: configureAttack(config)
    App->>PlatformFactory: createNetworkInterface()
    PlatformFactory->>App: WindowsNetworkInterface
    App->>PlatformFactory: createRawSocket()
    PlatformFactory->>App: WindowsRawSocket
    
    App->>NetworkInterface: getInterfaces()
    NetworkInterface->>App: interface list
    
    App->>NetworkInterface: resolveMacAddress(victim)
    NetworkInterface->>App: victim MAC
    
    App->>NetworkInterface: resolveMacAddress(target)
    NetworkInterface->>App: target MAC
    
    App->>RawSocket: open(interface)
    RawSocket->>App: success
    
    loop Attack Loop
        App->>RawSocket: sendPacket(arp_spoof)
        RawSocket->>Network: ARP packet
        Network->>RawSocket: network traffic
        RawSocket->>App: received packet
        App->>App: handlePacket()
    end
    
    User->>App: stopAttack()
    App->>RawSocket: sendPacket(restore_arp)
    App->>RawSocket: close()
```

## Design Patterns Used

1. **Singleton Pattern** - App class
2. **Factory Pattern** - PlatformFactory
3. **Strategy Pattern** - NetworkInterface and RawSocket abstractions
4. **RAII** - Resource management in all classes
5. **Observer Pattern** - Callback system for logging and stop events

## Key Features

- **Cross-platform abstraction** through PlatformFactory
- **Modern C++17** with smart pointers and atomics
- **Thread-safe** operations with atomic flags
- **Callback system** for extensibility
- **Comprehensive IP address** manipulation
- **Raw socket** networking capabilities 