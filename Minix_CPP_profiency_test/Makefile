OBJS = Parser.o

all: feed test_parser

feed: main.cc libparser.a
	g++ -g -std=c++17 -Igoogletest/googletest/include -pthread -o feed main.cc libparser.a -Lgoogletest/build/lib -lgtest -lgtest_main -pthread -lWs2_32

Parser.o: Parser.cc
	g++ -g -std=c++17 -Igoogletest/googletest/include -pthread -c -o Parser.o Parser.cc

libparser.a: $(OBJS)
	ar rcs libparser.a $(OBJS)

test_parser: test_parser.o libparser.a googletest/build/lib/libgtest.a googletest/build/lib/libgtest_main.a
	g++ -g -std=c++17 -Igoogletest/googletest/include -pthread -o test_parser test_parser.o libparser.a -Lgoogletest/build/lib -lgtest -lgtest_main -pthread -lWs2_32

test_parser.o: test_parser.cpp
	g++ -g -std=c++17 -Igoogletest/googletest/include -pthread -c -o test_parser.o test_parser.cpp

clean:
	rm -f *.o *.a feed test_parser
