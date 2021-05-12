#include "Graphics.h"
#include <iomanip>
#include <future>

using namespace graphics;

void draw_bytes(const uint8_t bytes[], const size_t size, const char *end = "\n") {
    for (size_t i = 0; i < size; i++) {
        // Print the bytes in hex
        printw("%02x", bytes[i]);
    }
    printw("%s", end);
}

// Helper function for drawing single bits on the screen (black or white pixels)
void draw_pixel(bool v) {
    const int col = COLOR_PAIR(v ? ColourCode::logical_white : ColourCode::logical_black);
    attron(col);

    // Add two blocks to make a square pixel
    addch(' ');
    addch(' ');

    attroff(col);
}

// Draws a simple line of x pixels
void draw_line(int x) {
    for (int i = 0; i < x; i++) {
        draw_pixel(true);
    }
    addch('\n');
}

void draw_HHDuc(const uint8_t HHDuc[], const uint16_t HDDuc_size) {
    attron(COLOR_PAIR(ColourCode::data));
    draw_bytes(HHDuc, HDDuc_size);
    attroff(COLOR_PAIR(ColourCode::data));
}

void draw_bqr(const uint8_t bqr[], uint16_t bqr_size) {
    auto print_with_colour = [bqr](int colour, size_t at, size_t size, bool end = false) {
        attron(COLOR_PAIR(colour));
        draw_bytes(&bqr[at], size, end ? "\n" : "");
        attroff(COLOR_PAIR(colour));
    };

    print_with_colour(ColourCode::magic, 0, 2);
    print_with_colour(ColourCode::ams, 2, 1);
    const auto data_size = static_cast<uint16_t>(bqr_size - 4);
    print_with_colour(ColourCode::data, 2, data_size);
    print_with_colour(ColourCode::checksum, static_cast<size_t>(3 + data_size - 1), 2, true);
}

void draw_qrcode(const QRcode *qr, int max_w, int max_h) {
    const int w = qr->width;

    // We need to divide by 2 for the maximum width, since each pixel is two characters big (horizontally)
    if ((w + settings::qr_margin_size * 2) + 1 > max_w / 2 || w + settings::qr_margin_size * 2 > max_h) {
        printw("Cannot display chipTAN QR code because the window is too small.\n");
        return;
    }

    if (!has_colors()) {
        printw("Cannot display chipTAN QR code because the terminal does not support colours.\n");
        return;
    }

    // Top border
    for (int i = 0; i < settings::qr_margin_size; i++) {
        draw_line(w + settings::qr_margin_size * 2);
    }

    for (int i = 0; i < w; i++) {
        // Left border
        for (int j = 0; j < settings::qr_margin_size; j++) {
            draw_pixel(true);
        }

        // Data
        for (int j = 0; j < w; j++) {
            // libqrencode gives us a 1 in the least significant bit if the segment is black
            draw_pixel(!static_cast<bool>(qr->data[(i * w) + j] & 1));
        }

        // Right border
        for (int j = 0; j < settings::qr_margin_size; j++) {
            draw_pixel(true);
        }

        addch('\n');
    }

    // Bottom border
    for (int i = 0; i < settings::qr_margin_size; i++) {
        draw_line(w + settings::qr_margin_size * 2);
    }
}

void draw_flicker_code(bool clock, const uint8_t half_byte) {
    constexpr uint8_t width = settings::flicker_code_segment_width * 5;
    constexpr uint8_t height = settings::flicker_code_segment_height;

    if (!has_colors()) {
        printw("Cannot display optical chipTAN flicker code because the terminal does not support colours.\n");
        return;
    }

    // Top border
    draw_line(width + 2);

    for (uint8_t i = 0; i < height; i++) {
        // Left border
        draw_pixel(true);

        // Data
        for (uint8_t j = 0; j < width; j++) {
            uint8_t field_idx = j / settings::flicker_code_segment_width;
            if (field_idx == 0) {
                // The first field is the clock bit
                draw_pixel(clock);
            } else {
                draw_pixel(static_cast<bool>((half_byte >> (field_idx - 1)) & 1));
            }
        }

        // Right border
        draw_pixel(true);

        addch('\n');
    }

    // Bottom border
    draw_line(width + 2);
}

void init_colours() {
    init_pair(ColourCode::magic, settings::magic_colour, COLOR_BLACK);
    init_pair(ColourCode::ams, settings::ams_colour, COLOR_BLACK);
    init_pair(ColourCode::data, settings::data_colour, COLOR_BLACK);
    init_pair(ColourCode::checksum, settings::checksum_colour, COLOR_BLACK);

    // These are inverted since we print spaces with a specific background colour
    init_pair(ColourCode::logical_white, COLOR_BLACK, settings::logical_white);
    init_pair(ColourCode::logical_black, COLOR_BLACK, settings::logical_black);
}

void graphics::graphics_loop(const uint8_t bqr[], const uint16_t bqr_size, const uint8_t HHDuc[], const uint16_t
HHDuc_size) {
    const WINDOW *window = initscr();
    noecho();
    curs_set(0);

    QRcode *bqr_code = QRcode_encodeData(bqr_size, bqr, 0, QR_ECLEVEL_L);

    if (has_colors()) {
        start_color();
        init_colours();
    }

    // Set the previous window dimensions to 0 so that the static content is drawn the first time
    int prev_win_w = 0, prev_win_h = 0;

    // The optical chipTAN readers require this start code at the beginning of each new transmission cycle
    constexpr uint8_t start_bytes_flicker_code[] = {0x0F, 0xFF};

    int flicker_code_x = 0, flicker_code_y = 0;

    uint16_t byte_idx = 0;
    bool clock = true;
    bool lsb = true;  // Flicker codes transmit LSB first

    const auto key_pressed_future = std::async(std::launch::async, &getch);
    while (key_pressed_future.wait_for(settings::flicker_code_delay) == std::future_status::timeout) {
        // Check if the window dimensions have changed
        // If so, clear the terminal and redraw all static content
        int win_h, win_w;
        getmaxyx(window, win_h, win_w);
        bool redraw = false;
        if (prev_win_w != win_w || prev_win_h != win_h) {
            prev_win_w = win_w;
            prev_win_h = win_h;
            redraw = true;

            clear();

            if (has_colors()) {
                printw("HHDuc: ");
                draw_HHDuc(HHDuc, HHDuc_size);
                printw("BQR:   ");
                draw_bqr(bqr, bqr_size);
            } else {
                printw("HHDuc: ");
                draw_bytes(HHDuc, HHDuc_size);
                printw("BQR:   ");
                draw_bytes(bqr, bqr_size);
            }
            addch('\n');

            getyx(window, flicker_code_y, flicker_code_x);
        }

        move(flicker_code_y, flicker_code_x);

        // For the first two byte indexes, display the starting code, after that use the HHDuc
        uint8_t byte;
        if (byte_idx < 2) {
            byte = start_bytes_flicker_code[byte_idx];
        } else {
            byte = HHDuc[byte_idx - 2];
        }

        // Flicker codes display each half byte for one complete clock cycle (on & off again) before transmitting the
        // next one
        draw_flicker_code(clock, lsb ? static_cast<uint8_t>(byte & 0xF) : static_cast<uint8_t>(byte >> 4));
        if (!clock) {
            if (!lsb && ++byte_idx - 2 >= HHDuc_size) byte_idx = 0;
            lsb = !lsb;
        }
        clock = !clock;

        addch('\n');

        if (redraw) {
            [[maybe_unused]] int curr_x;
            int curr_y;
            getyx(window, curr_y, curr_x);
            draw_qrcode(bqr_code, win_w, win_h - curr_y);

            printw("\nPress any key to exit.");
        }

        refresh();
    }

    QRcode_free(bqr_code);
    endwin();
}
