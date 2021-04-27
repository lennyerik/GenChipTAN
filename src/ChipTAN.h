#ifndef GENCHIPTAN_CHIPTAN_H
#define GENCHIPTAN_CHIPTAN_H

#include <string>
#include <stdexcept>

namespace chipTAN {

    // HHDuc
    constexpr uint16_t HHDuc_size(const std::string& startcode, const std::string& DE1, const std::string& DE2, const
    std::string& DE3) {
        size_t size = 4 + startcode.size();
        size += DE1.empty() ? 0 : (DE1.size() + 1);
        size += DE2.empty() ? 0 : (DE2.size() + 1);
        size += DE3.empty() ? 0 : (DE3.size() + 1);

        if (size - 1 > UINT8_MAX) throw std::length_error("The total size of the HHDuc must not exceed 256 bytes");

        return static_cast<uint16_t>(size);
    }
    void generate_HHDuc(const std::string& startcode, const std::string& DE1, const std::string& DE2, const
    std::string& DE3, uint8_t out[]);

    // BQR
    constexpr uint16_t bqr_size(const uint16_t HHDuc_size) {
        if (HHDuc_size - 1 > UINT8_MAX) throw std::invalid_argument("The HHDuc size must not exceed 256 bytes");
        return static_cast<uint16_t>(HHDuc_size + 5);
    }
    void generate_bqr_data(const uint8_t HHDuc[], uint16_t HHDuc_size, uint8_t out[]);

}

#endif //GENCHIPTAN_CHIPTAN_H
