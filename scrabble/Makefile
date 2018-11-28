CXX = g++
CPPFLAGS = -g -std=c++11 -Wall
B_DIR = bin

all: $(B_DIR)/.dirstamp scrabble

scrabble: scrabble.cpp $(B_DIR)/Player.o $(B_DIR)/Bag.o $(B_DIR)/Board.o $(B_DIR)/ConsolePrinter.o $(B_DIR)/Dictionary.o $(B_DIR)/Move.o $(B_DIR)/ExchangeMove.o $(B_DIR)/PassMove.o $(B_DIR)/PlaceMove.o
	$(CXX) $(CPPFLAGS) scrabble.cpp $(B_DIR)/Player.o $(B_DIR)/Bag.o $(B_DIR)/Board.o $(B_DIR)/ConsolePrinter.o $(B_DIR)/Dictionary.o $(B_DIR)/Move.o $(B_DIR)/ExchangeMove.o $(B_DIR)/PassMove.o $(B_DIR)/PlaceMove.o -o scrabble

$(B_DIR)/Player.o: Player.h Player.cpp
	$(CXX) $(CPPFLAGS) -c Player.cpp -o $(B_DIR)/Player.o

$(B_DIR)/Bag.o: Bag.h Bag.cpp
	$(CXX) $(CPPFLAGS) -c Bag.cpp -o $(B_DIR)/Bag.o

$(B_DIR)/Board.o: Board.h Board.cpp
	$(CXX) $(CPPFLAGS) -c Board.cpp -o $(B_DIR)/Board.o

$(B_DIR)/ConsolePrinter.o: ConsolePrinter.h ConsolePrinter.cpp
	$(CXX) $(CPPFLAGS) -c ConsolePrinter.cpp -o $(B_DIR)/ConsolePrinter.o
	
$(B_DIR)/Dictionary.o: Dictionary.h Dictionary.cpp
	$(CXX) $(CPPFLAGS) -c Dictionary.cpp -o $(B_DIR)/Dictionary.o
	
$(B_DIR)/Move.o: Move.h Move.cpp
	$(CXX) $(CPPFLAGS) -c Move.cpp -o $(B_DIR)/Move.o

$(B_DIR)/ExchangeMove.o: Move.h ExchangeMove.cpp
	$(CXX) $(CPPFLAGS) -c ExchangeMove.cpp -o $(B_DIR)/ExchangeMove.o
	
$(B_DIR)/PassMove.o: Move.h PassMove.cpp
	$(CXX) $(CPPFLAGS) -c PassMove.cpp -o $(B_DIR)/PassMove.o
	
$(B_DIR)/PlaceMove.o: Move.h PlaceMove.cpp
	$(CXX) $(CPPFLAGS) -c PlaceMove.cpp -o $(B_DIR)/PlaceMove.o
	
$(B_DIR)/.dirstamp:
	mkdir -p $(B_DIR)
	touch $(B_DIR)/.dirstamp
    
.PHONY: clean    
clean:
	rm -rf $(B_DIR)
