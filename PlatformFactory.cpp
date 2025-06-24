#include "PlatformAbstraction.hpp"
#include <memory>

#ifdef _WIN32
#include "WindowsPlatform.hpp"
#elif defined(__linux__)
#include "LinuxPlatform.hpp"
#elif defined(__APPLE__)
#include "MacOSPlatform.hpp"
#endif

std::unique_ptr<NetworkInterface> PlatformFactory::createNetworkInterface() {
#ifdef _WIN32
	return std::make_unique<WindowsNetworkInterface>();
#elif defined(__linux__)
	return std::make_unique<LinuxNetworkInterface>();
#elif defined(__APPLE__)
	return std::make_unique<MacOSNetworkInterface>();
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
#elif defined(__APPLE__)
	return std::make_unique<MacOSRawSocket>();
#else
	// Unsupported platform
	return nullptr;
#endif
} 