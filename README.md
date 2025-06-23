# ARP Spoofing Tool - C++

Modern implementation of ARP spoofing tool written in C++17, using smart pointers, atomics, callbacks and design patterns. The program is cross-platform and requires no external dependencies.

## 🎯 **Key Concepts**

### **Victim**
- This is a host that **does not know** it is being attacked
- Its network traffic is intercepted and redirected
- It thinks it is communicating directly with the target
- Example: user's computer on the network

### **Target**
- This is a host that the victim is trying to communicate with
- Can be a default gateway, server, or another computer
- Example: router, internet server, another computer on the network

### **One-way Mode**
- **Normal mode:** We spoof both victim and target
  - Victim thinks our computer is the target
  - Target thinks our computer is the victim
  - We intercept traffic in both directions

- **One-way mode:** We only spoof the victim
  - Victim thinks our computer is the target
  - Target is not spoofed
  - We only intercept traffic from victim to target
  - Traffic from target to victim goes normally

## 🏗️ **Architecture**

The program uses modern design patterns:

### **Singleton Pattern (App)**
- Central application point
- Manages attack lifecycle
- Coordinates all components

### **Abstraction Factory (PlatformFactory)**
- Creates platform-dependent components
- Enables easy extension to other systems
- Encapsulates differences between platforms

### **IPAddress Class**
Modern class for IPv4 address handling in SFML style:

```cpp
// Creating IP addresses
IPAddress ip1("192.168.1.1");
IPAddress ip2(192, 168, 1, 1);
IPAddress ip3 = IPAddress::fromString("192.168.1.1");

// Operations on addresses
if (ip1 == ip2) { /* ... */ }
if (ip1 < ip2) { /* ... */ }
IPAddress network = ip1 & mask;
IPAddress broadcast = ip1 | ~mask;

// Checking address types
if (ip1.isPrivate()) { /* ... */ }
if (ip1.isLocalhost()) { /* ... */ }
if (ip1.isPublic()) { /* ... */ }

// Conversions
std::string str = ip1.toString();
uint32_t num = ip1.toUint32();
std::vector<uint8_t> bytes = ip1.toBytes();

// Predefined addresses
IPAddress::Any        // 0.0.0.0
IPAddress::Localhost  // 127.0.0.1
IPAddress::Broadcast  // 255.255.255.255
```

## 📁 **File Structure**

```
arpspoof/
├── App.hpp/cpp                    # Main application class (Singleton)
├── IPAddress.hpp/cpp              # IPv4 address handling class
├── ArpSpoofer.hpp/cpp             # ARP attack execution class
├── PlatformAbstraction.hpp        # Platform abstractions
├── WindowsPlatform.hpp/cpp        # Windows implementation
├── PlatformFactory.cpp            # Component factory
├── NetworkHeaders.hpp             # Network header definitions
├── main.cpp                       # Main executable file
├── arpspoof.vcxproj               # Visual Studio project
└── README.md                      # This file
```

## 🚀 **Compilation**

### **Requirements**
- Visual Studio 2019/2022 with C++17
- Windows 10/11
- Administrator privileges

### **Compilation**
1. Open `arpspoof.vcxproj` in Visual Studio
2. Select Release x64 configuration
3. Compile project (Ctrl+Shift+B)

## 📖 **Usage**

### **Basic Syntax**
```bash
arpspoof.exe [OPTIONS] <victim-ip> [target-ip]
```

### **Options**
- `--help, -h` - Display help
- `--list, -l` - List network interfaces
- `--interface, -i` - Specify network interface
- `--oneway, -o` - One-way mode
- `--interval, -t` - ARP packet interval (seconds)
- `--verbose, -v` - Detailed logging

### **Examples**

**List interfaces:**
```bash
arpspoof.exe --list
```

**Basic attack (victim -> gateway):**
```bash
arpspoof.exe 192.168.1.10
```

**Attack with specific target:**
```bash
arpspoof.exe 192.168.1.10 192.168.1.1
```

**One-way mode:**
```bash
arpspoof.exe --oneway 192.168.1.10
```

**Specified interface and interval:**
```bash
arpspoof.exe -i "Ethernet" -t 5 192.168.1.10
```

## 🔧 **Features**

### **Automatic Interface Detection**
The program automatically detects the appropriate network interface based on the victim's IP address.

### **MAC Address Resolution**
Automatic MAC address resolution using ARP request packets.

### **Traffic Interception**
The program intercepts network traffic between victim and target and forwards it.

### **Safe Stop**
After pressing Ctrl+C, the program sends correct ARP packets to restore normal network operation.

## ⚠️ **Warnings**

- **Program requires administrator privileges!**
- Use only in controlled environment
- ARP spoofing can disrupt network operation
- Do not use for unauthorized traffic interception

## 🛡️ **Security**

The program implements the following security measures:

- IP address validation
- Permission checking
- Safe memory management (RAII)
- Error handling and exceptions
- Safe attack termination

## 🔮 **Extensions**

The program is designed for easy extension:

- **New platforms:** Add implementation in `PlatformAbstraction.hpp`
- **New protocols:** Extend `NetworkHeaders.hpp`
- **New features:** Add methods to `App` class
- **GUI:** Can add graphical interface using callbacks

## 📝 **License**

This project is intended for educational purposes and security testing in controlled environments only.

## 🤝 **Contributing**

The project is open to community contributions. Please:

- Report bugs
- Suggest new features
- Code improvements
- Documentation

---

**Note:** Use this tool responsibly and only in environments you control or have permission to test. 