CFLAG = -std=c++11 -O2 -w
CXX = g++

.PHONY : clean

wine : ai.o board.o main.o
	$(CXX) $^ -o $@
	chmod +x $@
	@echo wine is ready!
ai.o : AI.cpp AI.h Board.h
	$(CXX) AI.cpp $(CFLAG) -c -o $@
board.o : Board.cpp Board.h
	$(CXX) Board.cpp $(CFLAG) -c -o $@
main.o : main.cpp AI.h Board.h
	$(CXX) main.cpp $(CFLAG) -c -o $@
clean :
	-rm -rf *.o wine

