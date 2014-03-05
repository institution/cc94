
# *.cpp -> *.o
# `find . -name '*.cpp' | rev | cut -c4- | rev | sed -e 's/$/o/' | tr '\n' ' '`

OBJS=iomm.o env.o objs.o csv.o roll.o console.o renderer.o pixfont.o 


main: main.o ${OBJS}
	g++ main.o ${OBJS} -o main -I./include -L./lib -std=c++11 -g -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

test: test_ai.o ${OBJS}
	g++ test_ai.o ${OBJS} -o test -I./include -L./lib -std=c++11 -g -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network


#model: model.o iomm.o env.o
#	g++ model.o iomm.o env.o -o model -I./include -L./lib -std=c++0x -g -lsfml-network
		
${OBJS} main.o test_ai.o: %.o: %.cpp
	g++ -c -o $@ $< -std=c++11 -g

clean:
	rm -f main *.o

