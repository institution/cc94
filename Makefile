main: main.cpp
	g++ main.cpp -o main -I./include -L./lib -std=c++0x -g -lsfml-graphics -lsfml-window -lsfml-system 
	
clean:
	# rm main
	
