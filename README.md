# ARP Spoofing Tool

A modern, cross-platform ARP spoofing tool written in C++17. This tool demonstrates ARP spoofing attacks for educational and testing purposes.

## Features

- **Cross-platform support**: Windows, Linux, and macOS
- **Modern C++17**: Uses modern C++ features and best practices
- **Object-oriented design**: Clean architecture with proper abstraction
- **Raw socket support**: Direct network packet manipulation
- **Interface detection**: Automatic network interface discovery
- **MAC address resolution**: ARP table lookup and resolution
- **Drop mode**: Option to drop packets instead of forwarding (cuts internet)
- **Interactive mode**: Step-by-step configuration without command line arguments
- **Educational purpose**: Designed for learning network security concepts

## Architecture

The application uses a platform abstraction layer that provides:
- `NetworkInterface`: Interface for network operations
- `RawSocket`: Interface for raw socket operations
- Platform-specific implementations for Windows, Linux, and macOS

### Class Structure

```
App (Main application logic)
├── ArpSpoofer (ARP spoofing functionality)
├── PlatformFactory (Creates platform-specific objects)
├── NetworkInterface (Abstract interface)
│   ├── WindowsNetworkInterface (Windows implementation)
│   ├── LinuxNetworkInterface (Linux implementation)
│   └── MacOSNetworkInterface (macOS implementation)
└── RawSocket (Abstract interface)
    ├── WindowsRawSocket (Windows implementation)
    ├── LinuxRawSocket (Linux implementation)
    └── MacOSRawSocket (macOS implementation)
```

## Building

### Prerequisites

- **Windows**: Visual Studio 2019 or later with C++17 support
- **Linux**: GCC 7+ or Clang 5+ with C++17 support
- **macOS**: Xcode Command Line Tools or Clang 5+ with C++17 support

### Windows Build

1. Open `arpspoof.sln` in Visual Studio
2. Select your target platform (x86 or x64)
3. Build the solution (Ctrl+Shift+B)
4. Run as Administrator (required for raw socket access)

### Linux Build

1. **Install dependencies** (if needed):
   ```bash
   sudo apt-get update
   sudo apt-get install build-essential
   ```

2. **Compile the application**:
   ```bash
   make
   ```

3. **Install system-wide** (optional):
   ```bash
   sudo make install
   ```

4. **Alternative compilation**:
   ```bash
   g++ -std=c++17 -Wall -Wextra -O2 -D__linux__ \
       main.cpp App.cpp ArpSpoofer.cpp IPAddress.cpp \
       PlatformFactory.cpp LinuxPlatform.cpp \
       -o arpspoof
   ```

### macOS Build

1. **Install Xcode Command Line Tools** (if not already installed):
   ```bash
   xcode-select --install
   ```

2. **Compile the application**:
   ```bash
   make
   ```

3. **Install system-wide** (optional):
   ```bash
   sudo make install
   ```

4. **Alternative compilation**:
   ```bash
   clang++ -std=c++17 -Wall -Wextra -O2 -D__APPLE__ \
       main.cpp App.cpp ArpSpoofer.cpp IPAddress.cpp \
       PlatformFactory.cpp MacOSPlatform.cpp \
       -o arpspoof
   ```

### macOS Build with Xcode

1. **Open the Xcode project**:
   ```bash
   open arpspoof.xcodeproj
   ```

2. **Configure launch arguments** (optional):
   - Product → Scheme → Edit Scheme
   - Run → Arguments → Arguments Passed On Launch
   - Add: `en0 192.168.1.100 192.168.1.1`

3. **Build and run**:
   - Cmd + B (Build)
   - Cmd + R (Run)

**Note**: The Xcode project provides a professional development environment with debugging, profiling, and static analysis tools. See `XCODE_README.md` for detailed instructions.

## Usage

### Windows

```cmd
# Run as Administrator
arpspoof.exe [interface_name] [target_ip] [spoofed_ip]
```

### Linux

```bash
# Run with sudo (required for raw socket access)
sudo ./arpspoof [interface_name] [target_ip] [spoofed_ip]

# If installed system-wide
sudo arpspoof [interface_name] [target_ip] [spoofed_ip]
```

### macOS

```bash
# Run with sudo (required for BPF access)
sudo ./arpspoof [interface_name] [target_ip] [spoofed_ip]

# If installed system-wide
sudo arpspoof [interface_name] [target_ip] [spoofed_ip]
```

### Examples

```bash
# Spoof gateway (192.168.1.1) to target (192.168.1.100)
sudo ./arpspoof eth0 192.168.1.100 192.168.1.1

# Spoof target to gateway (reverse direction)
sudo ./arpspoof eth0 192.168.1.1 192.168.1.100

# On macOS, interface might be en0, en1, etc.
sudo ./arpspoof en0 192.168.1.100 192.168.1.1

# Drop mode - cut internet between devices
sudo ./arpspoof --drop 192.168.1.100

# Drop mode with specific target
sudo ./arpspoof --drop 192.168.1.100 192.168.1.1

# One-way drop mode (only from victim to target)
sudo ./arpspoof --drop --oneway 192.168.1.100

# Interactive mode
./arpspoof
```

## Drop Mode (Tryb Porzucania Pakietów)

Program obsługuje tryb porzucania pakietów (`--drop` lub `-d`), który **odcina internet** między urządzeniami poprzez porzucanie pakietów zamiast ich przekazywania.

### ⚠️ **Ostrzeżenie**
**UWAGA: Ta funkcjonalność odcina internet między urządzeniami!**
- Używaj tylko w kontrolowanym środowisku
- Upewnij się, że masz uprawnienia do testowania
- Może zakłócić normalną pracę sieci

### **Sposoby użycia:**
```bash
# Odcina internet między 192.168.1.100 a bramą domyślną
sudo ./arpspoof --drop 192.168.1.100

# Odcina internet między dwoma konkretnymi urządzeniami
sudo ./arpspoof --drop 192.168.1.100 192.168.1.1

# Tryb jednokierunkowy + porzucanie (tylko od ofiary do celu)
sudo ./arpspoof --drop --oneway 192.168.1.100
```

### **Statystyki:**
Program wyświetla statystyki porzuconych pakietów:
```
Statystyki: Wysłano 15 ARP, Odebrano 127, Porzucono 127 pakietów
```

Więcej informacji: [DROP_MODE_README.md](DROP_MODE_README.md)

## Security Notice