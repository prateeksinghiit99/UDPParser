#pragma once

#include <iostream>
#include <cstring>
#include <map>
#include <string>
#include <fstream>
#include <cstdint>
#include <winsock2.h>
#include <ws2tcpip.h>

class Parser {
  public:
    // date - the day on which the data being parsed was generated.
    // It is specified as an integer in the form yyyymmdd.
    // For instance, 18 Jun 2018 would be specified as 20180618.
    //
    // outputFilename - name of the file output events should be written to.
    Parser(int date, const std::string &outputFilename);

    ~Parser();

    // buf - points to a char buffer containing bytes from a single UDP packet.
    // len - length of the packet.
    void onUDPPacket(const char *buf, size_t len);

    // Process stored packets in correct order
    void processPackets();

    // Made Public for Unit tests
    void processAddOrder(const char* buf);
    void processOrderExecuted(const char* buf);
    void processOrderCancelled(const char* buf);
    void processOrderReplaced(const char* buf);

private:
    std::map<uint32_t, std::pair<const char*, size_t>> receivedPackets; // Key is seuence Number : {Data Pointer, Size}
    std::ofstream outputFile;
};
