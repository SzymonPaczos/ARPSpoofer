#include "PlatformAbstraction.hpp"
#include "WindowsPlatform.hpp"

std::unique_ptr<NetworkInterface> PlatformFactory::createNetworkInterface() {
#ifdef _WIN32
	return std::make_unique<WindowsNetworkInterface>();
#elif defined(__linux__)
	// TODO: Implementacja dla Linux
	return nullptr;
#elif defined(__APPLE__)
	// TODO: Implementacja dla macOS
	return nullptr;
#else
	return nullptr;
#endif
}

std::unique_ptr<RawSocket> PlatformFactory::createRawSocket() {
#ifdef _WIN32
	return std::make_unique<WindowsRawSocket>();
#elif defined(__linux__)
	// TODO: Implementacja dla Linux
	return nullptr;
#elif defined(__APPLE__)
	// TODO: Implementacja dla macOS
	return nullptr;
#else
	return nullptr;
#endif
} 