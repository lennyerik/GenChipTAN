#ifndef GENCHIPTAN_GRAPHICS_H
#define GENCHIPTAN_GRAPHICS_H

#include <ncurses.h>
#include <qrencode.h>
#include <cstdint>

namespace graphics {

    enum Colour {
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
