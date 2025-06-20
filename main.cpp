#include "ArpSpoofer.hpp"
#include <cstdio>
#include <cstring>
#include <windows.h>

////////////////////////////////////////////////////////////
/// \brief Globalna instancja ArpSpoofer
///
/// Zmienna globalna przechowująca instancję głównej klasy
/// ArpSpoofer. Jest używana przez obsługę sygnału Ctrl+C
/// do bezpiecznego zatrzymania ataku.
///
/// \see CtrlCHandler()
///
////////////////////////////////////////////////////////////
static ArpSpoofer* g_arpSpoofer = nullptr;

////////////////////////////////////////////////////////////
/// \brief Obsługa sygnału przerwania (Ctrl+C) w programie
///
/// Funkcja ta jest wywoływana przez system gdy użytkownik naciśnie
/// Ctrl+C w konsoli. Ustawia flagę zatrzymania w głównej klasie
/// ArpSpoofer, co powoduje bezpieczne zakończenie głównej pętli
/// programu i rozpoczęcie procedury "unspoofing".
///
/// Nazwa funkcji "CtrlCHandler" pochodzi od:
/// - "CtrlC" - oznacza kombinację klawiszy Ctrl+C
/// - "Handler" - oznacza obsługę/obsługiwanie sygnału
///
/// Funkcja jest rejestrowana w systemie przez SetConsoleCtrlHandler()
/// w funkcji main().
///
/// \param dwCtrlType Typ sygnału kontrolnego (CTRL_C_EVENT dla Ctrl+C)
///
/// \return BOOL TRUE jeśli sygnał został obsłużony, FALSE jeśli nie
///
/// \see main(), SetConsoleCtrlHandler()
///
////////////////////////////////////////////////////////////
BOOL WINAPI CtrlCHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT && g_arpSpoofer) {
		g_arpSpoofer->requestStop();
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////
/// \brief Wyświetla pomoc programu
///
/// Funkcja ta wyświetla w konsoli szczegółową pomoc dotyczącą
/// używania programu, w tym wszystkie dostępne opcje i przykłady
/// użycia.
///
/// \param program_name Nazwa programu (argv[0])
///
/// \see main()
///
////////////////////////////////////////////////////////////
void showHelp(const char* program_name) {
	fprintf(stderr, "Użycie: %s --list | [-i iface] [--oneway] victim-ip [target-ip]\n\n", program_name);
	fprintf(stderr, "Opcje:\n");
	fprintf(stderr, "  --list          Wyświetla listę dostępnych interfejsów sieciowych\n");
	fprintf(stderr, "  -i iface        Określa interfejs sieciowy (numer lub nazwa)\n");
	fprintf(stderr, "  --oneway        Tylko jednokierunkowy atak (victim -> target)\n");
	fprintf(stderr, "  victim-ip       Adres IP ofiary (wymagany)\n");
	fprintf(stderr, "  target-ip       Adres IP celu (opcjonalny, domyślnie brama)\n\n");
	fprintf(stderr, "Przykłady:\n");
	fprintf(stderr, "  %s --list\n", program_name);
	fprintf(stderr, "  %s 192.168.1.10\n", program_name);
	fprintf(stderr, "  %s -i 1 192.168.1.10\n", program_name);
	fprintf(stderr, "  %s --oneway 192.168.1.10\n", program_name);
	fprintf(stderr, "  %s 192.168.1.10 192.168.1.1\n\n", program_name);
	fprintf(stderr, "UWAGA: Program wymaga uprawnień administratora!\n");
}

////////////////////////////////////////////////////////////
/// \brief Parsuje argumenty wiersza poleceń
///
/// Funkcja ta analizuje argumenty przekazane do programu
/// i wyodrębnia wszystkie opcje oraz parametry. Obsługuje
/// wszystkie dostępne opcje programu.
///
/// \param argc Liczba argumentów
/// \param argv Tablica argumentów
/// \param victim_ip Referencja do stringa na adres IP ofiary
/// \param target_ip Referencja do stringa na adres IP celu
/// \param interface_spec Referencja do stringa na specyfikację interfejsu
/// \param oneway Referencja do flagi trybu jednokierunkowego
///
/// \return bool true jeśli parsowanie się powiodło,
///         false w przypadku błędu
///
/// \see main(), showHelp()
///
////////////////////////////////////////////////////////////
bool parseArguments(int argc, char* argv[], 
                   std::string& victim_ip, std::string& target_ip,
                   std::string& interface_spec, bool& oneway) {
	// Pomiń nazwę programu
	argc--;
	argv++;

	while (argc > 0) {
		if (argc >= 2 && !strcmp(argv[0], "-i")) {
			interface_spec = argv[1];
			argc -= 2;
			argv += 2;
			continue;
		}
		if (!strcmp(argv[0], "--oneway")) {
			oneway = true;
			argc--;
			argv++;
			continue;
		}
		if (victim_ip.empty()) {
			victim_ip = argv[0];
			argc--;
			argv++;
			continue;
		}
		if (target_ip.empty()) {
			target_ip = argv[0];
			argc--;
			argv++;
			continue;
		}
		fprintf(stderr, "Nieznany argument: %s\n", argv[0]);
		return false;
	}
	
	if (victim_ip.empty()) {
		fprintf(stderr, "Brak wymaganego argumentu, zobacz --help\n");
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////
/// \brief Główna funkcja programu - punkt wejścia aplikacji
///
/// Funkcja main() jest głównym punktem wejścia programu arpspoof.
/// Obsługuje parsowanie argumentów wiersza poleceń, inicjalizację
/// głównej klasy ArpSpoofer, przeprowadzenie ataku ARP spoofing
/// oraz bezpieczne zakończenie programu.
///
/// Nazwa funkcji "main" jest standardową nazwą głównej funkcji
/// w językach C/C++ - punkt wejścia do programu.
///
/// Program obsługuje następujące opcje:
/// - --help: wyświetla pomoc
/// - --list: wyświetla listę interfejsów
/// - -i: określa interfejs
/// - --oneway: tylko jednokierunkowy atak
/// - victim-ip: adres IP ofiary
/// - target-ip: adres IP celu (opcjonalny, domyślnie brama)
///
/// \param argc Liczba argumentów wiersza poleceń
/// \param argv Tablica wskaźników na argumenty wiersza poleceń
///
/// \return int 0 w przypadku sukcesu, 1 w przypadku błędu
///
/// \see ArpSpoofer, parseArguments(), showHelp()
///
////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	// Sprawdź argumenty
	if (argc < 2) {
		showHelp(argv[0]);
		return 1;
	}

	// Obsługa opcji --help
	if (argc == 2 && !strcmp(argv[1], "--help")) {
		showHelp(argv[0]);
		return 0;
	}

	// Obsługa opcji --list
	if (argc == 2 && !strcmp(argv[1], "--list")) {
		ArpSpoofer spoofer;
		spoofer.listInterfaces();
		return 0;
	}

	// Parsuj argumenty
	std::string victim_ip, target_ip, interface_spec;
	bool oneway = false;
	
	if (!parseArguments(argc, argv, victim_ip, target_ip, interface_spec, oneway)) {
		return 1;
	}

	// Utwórz instancję ArpSpoofer
	ArpSpoofer spoofer;
	g_arpSpoofer = &spoofer;

	// Zarejestruj obsługę sygnału Ctrl+C
	SetConsoleCtrlHandler(CtrlCHandler, TRUE);

	// Skonfiguruj atak
	if (!spoofer.configureAttack(victim_ip, target_ip, interface_spec, oneway)) {
		return 1;
	}

	// Rozpocznij atak
	if (!spoofer.startAttack()) {
		return 1;
	}

	return 0;
} 