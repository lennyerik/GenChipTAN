#include "ChipTAN.h"
#include <algorithm>

#define CRCPP_USE_CPP11

#include "CRC.h"


// HHDuc
uint8_t xor_checksum(const uint8_t bytes[], const size_t length) {
    uint8_t s = 0;
    for (size_t i = 0; i < length; i++) {
        s ^= bytes[i] >> 4;
        s ^= bytes[i] & 0xF;
    }
    return s;
}

// Sum of digits / "Quersumme"
constexpr uint8_t q(uint32_t n) {
    uint8_t result = 0;
    while (n != 0) {
        result += n % 10;
        n /= 10;
    }
    return result;
}

void
chipTAN::generate_HHDuc(const std::string &startcode, const std::string &DE1, const std::string &DE2, const
std::string &DE3, uint8_t out[]) {
    if (startcode.empty()) throw std::invalid_argument("The startcode must not be empty");
    if (startcode.size() > 0b00111111) throw std::length_error("Startcode length must not be greater than 0b00111111");

    const uint16_t size = chipTAN::HHDuc_size(startcode, DE1, DE2, DE3);

    // Helper functions for the Luhn-checksum, which is calculated in the end
    uint16_t luhnsum = 0;
    auto add_to_luhnsum = [&luhnsum](const uint8_t bytes[], const size_t length) {
        for (size_t i = 0; i < length; i++) {
            uint8_t b = bytes[i];
            luhnsum += (b >> 4) + q(static_cast<uint32_t>(2) * (b & 0xF));
        }
    };

    constexpr uint8_t control_byte = 1;

    out[0] = static_cast<uint8_t>(size - 1);  // LC
    out[1] = static_cast<uint8_t>(startcode.size() | (0b11 << 6));  // LS
    out[2] = control_byte;  // Control byte
    add_to_luhnsum(&control_byte, 1);
    uint8_t *current_pos = std::copy(startcode.begin(), startcode.end(), &out[3]); // Startcode
    add_to_luhnsum(&out[3], startcode.size());

    // Data
    auto maybe_append_DE = [&current_pos, add_to_luhnsum](const std::string &DE) {
        if (!DE.empty()) {
            if (DE.size() > 0b01111111) {
                throw std::length_error("Length of a data entry (DE) must not be bigger than 0b01111111");
            }
            *current_pos++ = static_cast<uint8_t>(DE.size() | (1 << 6)); // LDE
            uint8_t *previous_pos = current_pos;
            current_pos = std::copy(DE.begin(), DE.end(), current_pos); // DE
            add_to_luhnsum(previous_pos, DE.size());
        }
    };
    maybe_append_DE(DE1);
    maybe_append_DE(DE2);
    maybe_append_DE(DE3);

    // Checksums: Luhn-checksum over only a part of the HHDuc and xor checksum over all of it
    // The Luhn-checksum is now calculated in its final form using the luhnsum variable
    *current_pos = static_cast<uint8_t>(((10 - (luhnsum % 10)) % 10) << 4) | xor_checksum(out, current_pos - out);
}


// BQR
void chipTAN::generate_bqr_data(const uint8_t HHDuc[], const uint16_t HHDuc_size, uint8_t *out) {
    const size_t size = bqr_size(HHDuc_size);

    // Magic
    out[0] = 'D';
    out[1] = 'K';

    // AMS flag
    out[2] = 'N';

    // HHDuc data
    std::copy_n(HHDuc, HHDuc_size, &out[3]);

    // Checksum
    uint16_t checksum = CRC::Calculate(out, size - 2, CRC::CRC_16_ARC());
    out[size - 2] = static_cast<uint8_t>(checksum >> 8);
    out[size - 1] = static_cast<uint8_t>(checksum & 0xFF);

    // Starting after the magic number, xor everything with "DK"
    for (size_t i = 2; i < size; i += 2) {
        out[i] ^= 'D';
        if (i + 1 < size) {
            out[i + 1] ^= 'K';
        }
    }
}
