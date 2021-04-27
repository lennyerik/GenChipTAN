#include "ChipTAN.h"
#include <iostream>
#include <iomanip>
#ifdef COMPILE_WITH_CLI_GRAPHICS
#include <qrencode.h>
#include <ncurses.h>
#endif

void print_bytes_stdout(const uint8_t bytes[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(bytes[i]) << ' ';
    }
    std::cout << '\n';
}

void print_bytes_ncurses(const uint8_t bytes[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        printw("%02x ", bytes[i]);
    }
    printw("\n");
}

int main() {
    std::cout << "==========================" << '\n'
              << "       GenChipTAN" << '\n'
              << "       by lennyerik" << '\n'
              << "==========================" << "\n\n";

    std::cout << "HHDuc Setup" << '\n'
              << "--------------------------" << "\n\n"
              << "Please enter the parameters for the HHDuc structure." << '\n'
              << "The Startcode is mandatory, all other fields can be left empty" << '\n'
              << "by pressing enter without entering a value." << "\n\n"
              << "Example:" << '\n'
              << "Startcode: 82112345" << '\n'
              << "DE1: 0123456789" << '\n'
              << "DE2: 100,00" << '\n'
              << "DE3: (empty)" << "\n\n";

    std::string startcode, DE1, DE2, DE3;
    while (startcode.empty()) {
        std::cout << "Startcode: ";
        std::getline(std::cin, startcode);
    }
    std::cout << "DE1: ";
    std::getline(std::cin, DE1);
    std::cout << "DE2: ";
    std::getline(std::cin, DE2);
    std::cout << "DE3: ";
    std::getline(std::cin, DE3);

    std::cout << '\n';

    // HHDuc
    const uint16_t HHDuc_size = chipTAN::HHDuc_size(startcode, DE1, DE2, DE3);
    auto HHDuc = new uint8_t[HHDuc_size];
    chipTAN::generate_HHDuc(startcode, DE1, DE2, DE3, HHDuc);

    // BQR
    const uint16_t bqr_size = chipTAN::bqr_size(HHDuc_size);
    auto bqr = new uint8_t[bqr_size];
    chipTAN::generate_bqr_data(HHDuc, HHDuc_size, bqr);

#ifdef COMPILE_WITH_CLI_GRAPHICS
    initscr();
    printw("HHDuc: ");
    print_bytes_ncurses(HHDuc, HHDuc_size);
    printw("BQR:   ");
    print_bytes_ncurses(bqr, bqr_size);
    printw("\nPress any key to exit.");
    refresh();
    getch();
    endwin();
#else
    std::cout << "HHDuc: ";
    print_bytes_stdout(HHDuc, HHDuc_size);

    std::cout << "BQR:   ";
    print_bytes_stdout(bqr, bqr_size);
#endif

    delete []bqr;
    delete []HHDuc;

    return 0;
}
