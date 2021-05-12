#ifndef GENCHIPTAN_GRAPHICS_H
#define GENCHIPTAN_GRAPHICS_H

#include <ncurses.h>
#include <qrencode.h>
#include <cstdint>
#include <chrono>

namespace graphics {

    namespace settings {
        // Flicker code
        constexpr uint8_t flicker_code_segment_width = 3;
        constexpr uint8_t flicker_code_segment_height = 5;
        constexpr auto flicker_code_delay = std::chrono::milliseconds(20);

        // QR code
        constexpr uint8_t qr_margin_size = 4;

        // Colours
        constexpr int magic_colour = COLOR_RED;
        constexpr int ams_colour = COLOR_GREEN;
        constexpr int data_colour = COLOR_YELLOW;
        constexpr int checksum_colour = COLOR_BLUE;
        constexpr int logical_white = COLOR_WHITE;
        constexpr int logical_black = COLOR_BLACK;
    }

    // ncurses colour pair codes
    enum ColourCode {
        magic = 1,
        ams = 2,
        data = 3,
        checksum = 4,

        // Used for flicker and QR code generation
        logical_white = 5,
        logical_black = 6
    };

    void graphics_loop(const uint8_t bqr[], uint16_t bqr_size, const uint8_t HHDuc[], uint16_t HHDuc_size);

}

#endif //GENCHIPTAN_GRAPHICS_H
