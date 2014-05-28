CXXFLAGS = -g

.PHONY: all check version_autogen.h

all: test/board_test test/genmove_test test/random_move test/incheck_test engine

test/board_test: board.o test/board_test.o
	g++ $(CXXFLAGS) -o test/board_test board.o test/board_test.o

test/genmove_test: board.o test/genmove_test.o
	g++ $(CXXFLAGS) -o test/genmove_test board.o test/genmove_test.o

test/random_move: board.o test/random_move.o
	g++ $(CXXFLAGS) -o test/random_move board.o test/random_move.o

test/incheck_test: board.o test/incheck_test.o
	g++ $(CXXFLAGS) -o test/incheck_test board.o test/incheck_test.o

version_autogen.h:
	./version_autogen.sh

uci.o: version_autogen.h

engine: board.o uci.o search.o
	g++ $(CXXFLAGS) -o engine uci.o board.o search.o

check: test/board_test test/genmove_test test/incheck_test
	./test/board_test
	./test/incheck_test
	./test/genmove_test

clean:
	rm -f *.o *.d test/*.o test/*.d 
	rm -f test/board_test test/genmove_test test/incheck_test
	rm -f test/random_move
	rm -f engine
	rm -f *~ test/*~
	rm -f version_autogen.h
	
-include *.d

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -MM -MF $(patsubst %.o,%.d,$@) -o $@ $<
	$(CXX) $(CXXFLAGS) -c -o $@ $<

