
# *.cpp -> *.o
# `find . -name '*.cpp' | rev | cut -c4- | rev | sed -e 's/$/o/' | tr '\n' ' '`

OBJS=b/iomm.o b/env.o b/objs.o b/csv.o b/roll.o b/console.o b/renderer.o b/pixfont.o b/ai.o b/ox.o b/terr.o b/unit.o b/envgame.o b/build.o b/player.o


OPTS=-g -fno-inline-functions -lboost_serialization

main: b/main.o ${OBJS}
	g++ b/main.o ${OBJS} -o main -I./src -L./lib -std=c++11 ${OPTS} -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

test: b/test.o ${OBJS}
	g++ b/test.o ${OBJS} -o test -I./src -L./lib -std=c++11 ${OPTS} -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

#test_ox: b/test_ox.o ${OBJS}
#	g++ b/test_ox.o ${OBJS} -o test_ox -I./src -L./lib -std=c++11 -g -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

#model: model.o iomm.o env.o
#	g++ model.o iomm.o env.o -o model -I./include -L./lib -std=c++0x -g -lsfml-network

b/main.o b/test.o b/test_ox.o: b/%.o: src/%.cpp
	g++ -c -o $@ $< -std=c++11 ${OPTS}
		
${OBJS}: b/%.o: src/%.cpp src/%.h
	g++ -c -o $@ $< -std=c++11 ${OPTS}

clean:
	rm -f b/*.o main core.* test

