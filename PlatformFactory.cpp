#include "PlatformAbstraction.hpp"
#include <memory>

#ifdef _WIN32
#include "WindowsPlatform.hpp"
#elif defined(__linux__)
#include "LinuxPlatform.hpp"
#endif

std::unique_ptr<NetworkInterface> PlatformFactory::createNetworkInterface() {
#ifdef _WIN32
	return std::make_unique<WindowsNetworkInterface>();
#elif defined(__linux__)
	return std::make_unique<LinuxNetworkInterface>();
#else
	// Unsupported platform
	return nullptr;
#endif
}

std::unique_ptr<RawSocket> PlatformFactory::createRawSocket() {
#ifdef _WIN32
	return std::make_unique<WindowsRawSocket>();
#elif defined(__linux__)
	return std::make_unique<LinuxRawSocket>();
#else
	// Unsupported platform
	return nullptr;
#endif
} 