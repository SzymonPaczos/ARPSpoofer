# Makefile for ARP Spoofing Tool
# Compiles on Linux, macOS, and Windows platforms

# Detect platform
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
    CXX = g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -D_WIN32
    LDFLAGS = -lws2_32 -liphlpapi
    SOURCES = main.cpp \
              App.cpp \
              ArpSpoofer.cpp \
              IPAddress.cpp \
              PlatformFactory.cpp \
              WindowsPlatform.cpp
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = MACOS
        CXX = clang++
        CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -D__APPLE__
        LDFLAGS = 
        SOURCES = main.cpp \
                  App.cpp \
                  ArpSpoofer.cpp \
                  IPAddress.cpp \
                  PlatformFactory.cpp \
                  MacOSPlatform.cpp
    else
        PLATFORM = LINUX
        CXX = g++
        CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -D__linux__
        LDFLAGS = 
        SOURCES = main.cpp \
                  App.cpp \
                  ArpSpoofer.cpp \
                  IPAddress.cpp \
                  PlatformFactory.cpp \
                  LinuxPlatform.cpp
    endif
endif

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = arpspoof

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (requires root privileges for raw socket access)
install: $(TARGET)
ifeq ($(PLATFORM),WINDOWS)
	@echo "Installation on Windows not supported via makefile"
	@echo "Please copy $(TARGET).exe to a directory in your PATH"
else
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +s /usr/local/bin/$(TARGET)
endif

# Uninstall
uninstall:
ifeq ($(PLATFORM),WINDOWS)
	@echo "Uninstallation on Windows not supported via makefile"
else
	sudo rm -f /usr/local/bin/$(TARGET)
endif

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Show platform info
info:
	@echo "Detected platform: $(PLATFORM)"
	@echo "Compiler: $(CXX)"
	@echo "Compiler flags: $(CXXFLAGS)"
	@echo "Linker flags: $(LDFLAGS)"
	@echo "Source files: $(SOURCES)"

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the application (default)"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to /usr/local/bin (requires sudo, Linux/macOS only)"
	@echo "  uninstall- Remove from /usr/local/bin (Linux/macOS only)"
	@echo "  info     - Show platform and build information"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Supported platforms: Linux, macOS, Windows"

.PHONY: all clean install uninstall debug release help info 