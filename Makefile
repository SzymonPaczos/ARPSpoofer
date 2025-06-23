# Makefile for ARP Spoofing Tool
# Compiles on Linux platform

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -D__linux__
LDFLAGS = 

# Source files
SOURCES = main.cpp \
          App.cpp \
          ArpSpoofer.cpp \
          IPAddress.cpp \
          PlatformFactory.cpp \
          LinuxPlatform.cpp

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
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +s /usr/local/bin/$(TARGET)

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the application (default)"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  help     - Show this help message"

.PHONY: all clean install uninstall debug release help 