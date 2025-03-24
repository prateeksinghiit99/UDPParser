#include "Parser.h"

#include <cstdio>

#include <cstdint>

#include <fcntl.h>
#include <unistd.h>
//#include <arpa/inet.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <Windows.h>
#include <iostream>
//#include <sys/uio.h>

const char *inputFile = "test.in";

int main(int argc, char **argv) {
    std::cout << "Attach to Process for Debugging and Press Enter to continue...";
    std::cin.get();
    constexpr int currentDate = 20231205;
    Parser myParser(currentDate, "OutputFile");

    int fd = open(inputFile, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Couldn't open %s\n", inputFile);
        return 1;
    }

    char bigbuf[5000];
    while (read(fd, bigbuf, 2) != 0) {
        uint16_t packetSize = htons(*(uint16_t *)bigbuf);
        read(fd, bigbuf + 2, packetSize - 2);

        myParser.onUDPPacket(bigbuf, packetSize);
    }

    close(fd);

    // Process stored packets in order
    myParser.processPackets();

    return 0;
}
