
# *.cpp -> *.o
# `find . -name '*.cpp' | rev | cut -c4- | rev | sed -e 's/$/o/' | tr '\n' ' '`
OBJS=iomm.o text.o

main: main.o iomm.o text.o
	g++ main.o iomm.o text.o -o main -I./include -L./lib -std=c++0x -g -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

model: model.o iomm.o 
	g++ model.o iomm.o -o model -I./include -L./lib -std=c++0x -g -lsfml-network
		
${OBJS} main.o model.o: %.o: %.cpp
	g++ -c -o $@ $< -std=c++0x -g

clean:
	rm -f main *.o

