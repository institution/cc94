
# *.cpp -> *.o
# `find . -name '*.cpp' | rev | cut -c4- | rev | sed -e 's/$/o/' | tr '\n' ' '`


main: main.o iomm.o text.o env.o objs.o csv.o
	g++ main.o iomm.o text.o env.o objs.o csv.o -o main -I./include -L./lib -std=c++11 -g -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

#model: model.o iomm.o env.o
#	g++ model.o iomm.o env.o -o model -I./include -L./lib -std=c++0x -g -lsfml-network
		
iomm.o text.o main.o env.o objs.o csv.o: %.o: %.cpp
	g++ -c -o $@ $< -std=c++11 -g

clean:
	rm -f main *.o

