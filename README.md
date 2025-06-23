# ARP Spoofing Tool

A modern, cross-platform ARP spoofing tool written in C++17. This tool demonstrates ARP spoofing attacks for educational and testing purposes.

## Features

- **Cross-platform support**: Windows and Linux
- **Modern C++17**: Uses modern C++ features and best practices
- **Object-oriented design**: Clean architecture with proper abstraction
- **Raw socket support**: Direct network packet manipulation
- **Interface detection**: Automatic network interface discovery
- **MAC address resolution**: ARP table lookup and resolution
- **Educational purpose**: Designed for learning network security concepts

## Architecture

The application uses a platform abstraction layer that provides:
- `NetworkInterface`: Interface for network operations
- `RawSocket`: Interface for raw socket operations
- Platform-specific implementations for Windows and Linux

### Class Structure

```
App (Main application logic)
├── ArpSpoofer (ARP spoofing functionality)
├── PlatformFactory (Creates platform-specific objects)
├── NetworkInterface (Abstract interface)
│   ├── WindowsNetworkInterface (Windows implementation)
│   └── LinuxNetworkInterface (Linux implementation)
└── RawSocket (Abstract interface)
    ├── WindowsRawSocket (Windows implementation)
    └── LinuxRawSocket (Linux implementation)
```

## Building

### Prerequisites

- **Windows**: Visual Studio 2019 or later with C++17 support
- **Linux**: GCC 7+ or Clang 5+ with C++17 support

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

### Examples

```bash
# Spoof gateway (192.168.1.1) to target (192.168.1.100)
sudo ./arpspoof eth0 192.168.1.100 192.168.1.1

# Spoof target to gateway (reverse direction)
sudo ./arpspoof eth0 192.168.1.1 192.168.1.100
```

## Security Notice

⚠️ **WARNING**: This tool is for educational purposes only. Using ARP spoofing on networks you don't own or have explicit permission to test is illegal in many jurisdictions.

## Platform Support

### Windows
- Uses WinPcap/Npcap for raw socket access
- Requires Administrator privileges
- Supports Windows 10/11

### Linux
- Uses native Linux socket API
- Requires root privileges (sudo)
- Supports most Linux distributions

## Development

### Project Structure

```
arpa/
├── main.cpp                 # Entry point
├── App.hpp/cpp             # Main application class
├── ArpSpoofer.hpp/cpp      # ARP spoofing logic
├── IPAddress.hpp/cpp       # IP address utilities
├── PlatformAbstraction.hpp # Abstract interfaces
├── PlatformFactory.cpp     # Factory for platform objects
├── WindowsPlatform.hpp/cpp # Windows implementation
├── LinuxPlatform.hpp/cpp   # Linux implementation
├── NetworkHeaders.hpp      # Network protocol headers
├── arpspoof.sln           # Visual Studio solution
├── arpspoof.vcxproj       # Visual Studio project
├── Makefile               # Linux build script
├── README.md              # This file
└── UML_Diagram.md         # Class diagram
```

### Adding New Platforms

To add support for a new platform:

1. Create platform-specific header and implementation files
2. Implement `NetworkInterface` and `RawSocket` interfaces
3. Update `PlatformFactory.cpp` to include the new platform
4. Add appropriate preprocessor definitions

## License

This project is for educational purposes. Use responsibly and only on networks you own or have permission to test.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on both Windows and Linux
5. Submit a pull request

## Troubleshooting

### Common Issues

**Windows:**
- "Access denied" - Run as Administrator
- "WinPcap not found" - Install Npcap

**Linux:**
- "Permission denied" - Use sudo
- "Interface not found" - Check interface name with `ip link show`
- "Operation not permitted" - Ensure CAP_NET_RAW capability

### Debug Mode

**Linux:**
```bash
make debug
sudo ./arpspoof [args]
```

**Windows:**
Build in Debug configuration in Visual Studio 