#include "Parser.h"

Parser::Parser(int date, const std::string &outputFilename)
{
    outputFile.open(outputFilename, std::ios::binary | std::ios::app);
    if (!outputFile) {
        std::cerr << "Error: Could not open output file\n";
        exit(1);
    }
}

Parser::~Parser() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

void Parser::onUDPPacket(const char *buf, size_t len)
{   
    uint16_t packetSize = ntohs(*(uint16_t*)buf);
    uint32_t sequenceNumber = ntohl(*(uint32_t*)(buf + 2));

    if (receivedPackets.count(sequenceNumber) == 0) {
        receivedPackets[sequenceNumber] = { buf + 6, len - 6 }; // Store packet data
    }
}

void Parser::processPackets() {
    for (auto& entry : receivedPackets) {
        const char* buf = entry.second.first;
        size_t len = entry.second.second;

        // A UDP packet can have mulitple messages
        while (len > 0) {
            char msgType = buf[0];
            switch (msgType) {
            case 'A':   // Add order
                processAddOrder(buf);
                buf += 34;
                len -= 34;
                break;

            case 'E':   // Execute order
                processOrderExecuted(buf);
                buf += 21;
                len -= 21;
                break;

            case 'X':   // Cancel order
                processOrderCancelled(buf);
                buf += 21;
                len -= 21;
                break;

            case 'R':   // Replace order
                processOrderReplaced(buf);
                buf += 33;
                len -= 33;
                break;

            default:
                std::cerr << "Unknown message type: " << msgType << "\n";
                return;  // Exit if an unknown message is found
            }
        }
    }
}

void Parser::processAddOrder(const char* buf) {
    char output[44] = {};

    *(uint16_t*)(output) = htons(0x01);
    *(uint16_t*)(output + 2) = htons(44);
    memcpy(output + 4, buf + 22, 8);

    uint64_t timestamp, orderRef;
    uint32_t size;
    double price;

    memcpy(&timestamp, buf + 1, 8);
    memcpy(&orderRef, buf + 9, 8);
    memcpy(&size, buf + 18, 4);
    memcpy(&price, buf + 30, 8);

    memcpy(output + 12, &timestamp, 8);
    memcpy(output + 20, &orderRef, 8);
    output[28] = buf[17];
    memset(output + 29, 0, 3);
    memcpy(output + 32, &size, 4);
    memcpy(output + 36, &price, 8);

    outputFile.write(output, sizeof(output));
}

void Parser::processOrderExecuted(const char* buf) {
    char output[36] = {};

    *(uint16_t*)(output) = htons(0x02);
    *(uint16_t*)(output + 2) = htons(36);

    uint64_t timestamp, orderRef;
    uint32_t size;
    memset(output + 32, 0, 4);  // Execution price default 0

    memcpy(&timestamp, buf + 1, 8);
    memcpy(&orderRef, buf + 9, 8);
    memcpy(&size, buf + 17, 4);

    memcpy(output + 12, &timestamp, 8);
    memcpy(output + 20, &orderRef, 8);
    memcpy(output + 28, &size, 4);

    outputFile.write(output, sizeof(output));
}

void Parser::processOrderCancelled(const char* buf) {
    char output[32] = {};

    *(uint16_t*)(output) = htons(0x03);
    *(uint16_t*)(output + 2) = htons(32);

    uint64_t timestamp, orderRef;
    uint32_t sizeRemaining;

    memcpy(&timestamp, buf + 1, 8);
    memcpy(&orderRef, buf + 9, 8);
    memcpy(&sizeRemaining, buf + 17, 4);

    memcpy(output + 12, &timestamp, 8);
    memcpy(output + 20, &orderRef, 8);
    memcpy(output + 28, &sizeRemaining, 4);

    outputFile.write(output, sizeof(output));
}

void Parser::processOrderReplaced(const char* buf) {
    char output[48] = {};

    *(uint16_t*)(output) = htons(0x04);
    *(uint16_t*)(output + 2) = htons(48);

    uint64_t timestamp, oldOrderRef, newOrderRef;
    uint32_t newSize, newPrice;

    memcpy(&timestamp, buf + 1, 8);
    memcpy(&oldOrderRef, buf + 9, 8);
    memcpy(&newOrderRef, buf + 28, 8);
    memcpy(&newSize, buf + 36, 4);
    memcpy(&newPrice, buf + 40, 4);

    memcpy(output + 12, &timestamp, 8);
    memcpy(output + 20, &oldOrderRef, 8);
    memcpy(output + 28, &newOrderRef, 8);
    memcpy(output + 36, &newSize, 4);
    memcpy(output + 40, &newPrice, 4);

    outputFile.write(output, sizeof(output));
}