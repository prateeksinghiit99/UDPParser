#include <gtest/gtest.h>
#include "Parser.h"
#include <fstream>
#include <vector>
#include <cstring>

TEST(ParserTest, AddOrderMessage) {
    const char* filename = "test_output.out";
    std::remove(filename);

    {
        Parser parser(20231205, filename);

        // Test Input Data
        char testBuf[34] = {};
        testBuf[0] = 'A';
        uint64_t timestamp = 1700000000000;  // Big timestamp
        uint64_t orderRef = 1;
        char side = 'B';
        uint32_t size = htonl(100);
        char ticker[8] = {};
        memcpy(ticker, "MSFT", 4);
        double price = 99.0;

        // Copying it to testBuf to send
        memcpy(testBuf + 1, &timestamp, sizeof(timestamp));
        memcpy(testBuf + 9, &orderRef, sizeof(orderRef));
        testBuf[17] = side;
        memcpy(testBuf + 18, &size, sizeof(size));
        memcpy(testBuf + 22, ticker, sizeof(ticker));
        memcpy(testBuf + 30, &price, sizeof(price));

        // Testing this Method
        parser.processAddOrder(testBuf);
    }

    std::ifstream file(filename, std::ios::binary);

    std::vector<char> outputData((std::istreambuf_iterator<char>(file)), {});
    file.close();

    ASSERT_EQ(outputData.size(), 44);

    uint16_t msgType, msgSize;
    char outTicker[8] = {};
    uint64_t outTimestamp, outOrderRef;
    char outSide;
    uint32_t outSize;
    double outPrice;
    char padding[4] = {};

    size_t offset = 0;
    memcpy(&msgType, &outputData[offset], sizeof(msgType)); offset += 2;
    memcpy(&msgSize, &outputData[offset], sizeof(msgSize)); offset += 2;
    memcpy(outTicker, &outputData[offset], sizeof(outTicker)); offset += 8;
    memcpy(&outTimestamp, &outputData[offset], sizeof(outTimestamp)); offset += 8;
    memcpy(&outOrderRef, &outputData[offset], sizeof(outOrderRef)); offset += 8;
    memcpy(&outSide, &outputData[offset], sizeof(outSide)); offset += 1;
    memcpy(padding, &outputData[offset], sizeof(padding)); offset += 3;
    memcpy(&outSize, &outputData[offset], sizeof(outSize)); offset += 4;
    memcpy(&outPrice, &outputData[offset], sizeof(outPrice));

    msgType = ntohs(msgType);
    msgSize = ntohs(msgSize);
    outSize = ntohl(outSize);

    ASSERT_EQ(msgType, 0x01);
    ASSERT_EQ(msgSize, 44);
    ASSERT_EQ(outTimestamp, 1700000000000);
    ASSERT_EQ(outOrderRef, 1);
    ASSERT_EQ(outSide, 'B');
    ASSERT_STREQ(outTicker, "MSFT");
    ASSERT_EQ(outSize, 100);
    ASSERT_DOUBLE_EQ(outPrice, 99);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
