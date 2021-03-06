# GenChipTAN

The [chipTAN](https://en.wikipedia.org/wiki/Transaction_authentication_number#ChipTAN_/_Sm@rt-TAN_/_CardTAN) system is
used by German banks for authenticating online banking transactions.
The one-time codes (or TANs) are normally generated by a phone app or a dedicated physical device.
These devices (chipTAN readers) can work in  multiple different ways, but two of the most prominent systems are optical
chipTAN and chipTAN QR.  
This C++ program is able to generate both optical chipTAN and chipTAN QR codes that can be read by these devices.

![GenChipTAN](img/GenChipTAN.gif)

## Building & Running
GenChipTAN has only been tested on Linux.
MacOS is probably supported, although not tested (yet).
Windows support would require a bit of work, since ncurses (used for the graphics) is not available under Windows.  
The project is built using CMake:

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .

To run, just start the generated executable:

    ./GenChipTAN

## How does chipTAN work?
The protocols are all documented rather extensively:
* https://www.fiduciagad.de/content/dam/f8998-4/GAD-internet_neu_1/service-infothek/Download/PDS2-xs2a-PDFs/HHD-enhancement%20for%20optical%20interfaces%20V1.5.1%2020180605_EN_20190515.pdf
* https://www.hbci-zka.de/dokumente/spezifikation_deutsch/hhd/Belegungsrichtlinien%20TANve1.5%20FV%20vom%202018-04-16.pdf (German)
* https://6xq.net/flickercodes/ (also in German, although it includes Javascript code for generating flicker codes)

### Useful terminology for working on the code
* **HHDuc (HandHeld Device Universal optical Coupling)**: The general encoded data stream sent to any chipTAN reader
* **BQR (Banking QR Code)**: A special encoding / container for the HHDuc for use in a QR code
* **(optical chipTAN) flicker codes**: Used for transmitting raw HHDucs to optical chipTAN readers