# *.cpp -> *.o
# `find . -name '*.cpp' | rev | cut -c4- | rev | sed -e 's/$/o/' | tr '\n' ' '`
OBJS=main.o iomm.o text.o

main: ${OBJS}
	/usr/local/bin/cog.py main.cpp > main.tmp~
	cat main.tmp~ > main.cpp
	g++ ${OBJS} -o main -I./include -L./lib -std=c++0x -g -lsfml-graphics -lsfml-window -lsfml-system 

${OBJS}: %.o: %.cpp
	g++ -c -o $@ $< -std=c++0x -g

clean:
	rm -f main *.o

