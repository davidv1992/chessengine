CXXFLAGS = -O2

.PHONY: all check

all: test/board_test test/genmove_test test/random_move test/incheck_test

test/board_test: board.o test/board_test.o
	g++ $(CXXFLAGS) -o test/board_test board.o test/board_test.o

test/genmove_test: board.o test/genmove_test.o
	g++ $(CXXFLAGS) -o test/genmove_test board.o test/genmove_test.o

test/random_move: board.o test/random_move.o
	g++ $(CXXFLAGS) -o test/random_move board.o test/random_move.o

test/incheck_test: board.o test/incheck_test.o
	g++ $(CXXFLAGS) -o test/incheck_test board.o test/incheck_test.o

check: test/board_test test/genmove_test test/incheck_test
	./test/board_test
	./test/incheck_test
	./test/genmove_test

clean:
	rm -f *.o *.d test/*.o test/*.d 
	rm -f test/board_test test/genmove_test test/incheck_test
	rm -f test/random_move
	rm -f *~ test/*~
	
-include *.d

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -MM -MF $(patsubst %.o,%.d,$@) -o $@ $<
	$(CXX) $(CXXFLAGS) -c -o $@ $<

