# $@ -- The file name of the target of the rule.
# $* -- The stem with which an implicit rule matches
# $< -- The name of the first prerequisite.
# $^ -- The names of all the prerequisites
# $(VARS:%.cpp=%.o) -- pattern replace

# output files
OUTS:=main test

# compiler options
CCOPTS:=-I./src -std=c++11 -g -O0

# linker options
LLOPTS:=-lboost_serialization -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network


# list of compiled source b/fname.cpp.obj
OBJS:=$(shell find src -name '*.cpp')
OBJS:=$(OBJS:src/%.cpp=b/%.cpp.obj)

-include $(OBJS:%.obj=%.d)

# compiler
${OBJS}: b/%.obj: src/%
	g++ -c -MMD -MP -o $@ $< ${CCOPTS}
	
# linker
#${OUTS}: $(OBJS)
#	g++ -o $@ $^ ${LLOPTS}

clean:
	rm -f b/* ${OUTS} core.*
	

# test
TEST_D:=$(shell g++ -MM src/test.cpp -std=c++11 | ./deps.py)
TEST_D:=$(TEST_D:src/%.RRR=b/%.cpp.obj)

test: ${TEST_D}
	g++ -o $@ $^ ${LLOPTS}
	
# main
MAIN_D:=$(shell g++ -MM src/main.cpp -std=c++11 | ./deps.py)
MAIN_D:=$(MAIN_D:src/%.RRR=b/%.cpp.obj)

main: ${MAIN_D}
	g++ -o $@ $^ ${LLOPTS}
	
	
