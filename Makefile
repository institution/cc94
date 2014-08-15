# $@ -- The file name of the target of the rule.
# $* -- The stem with which an implicit rule matches
# $< -- The name of the first prerequisite.
# $^ -- The names of all the prerequisites
# $(VARS:%.cpp=%.o) -- pattern replace

CC:=g++-4.9

# output files
OUTS:=client/main server/test client/test format/test_format

# temporary dont build following files
IGNORE_SRC:=src/tree.cpp

WARNOPTS:=-Wsign-compare -Wreturn-type -Wparentheses -Wpedantic -Wconversion-null
# -Wall -Wextra 

# compiler options
CCOPTS:=-I./inc -I./src/format -I./src/server -std=c++11 -fmax-errors=5 -g -O0 ${WARNOPTS}
DEOPTS:=-g -O0

# linker options
LLOPTS:=-lboost_serialization -lboost_program_options -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network


# assert dirs
$(shell mkdir -p b)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p b/{})
$(shell mkdir -p bin)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p bin/{})

# list of compiled source b/fname.cpp.obj
OBJS:=$(shell find src -name '*.cpp')
OBJS:=$(filter-out $(IGNORE_SRC),$(OBJS))
OBJS:=$(OBJS:src/%.cpp=b/%.cpp.obj)

-include $(OBJS:%.obj=%.d)

# compiler
${OBJS}: b/%.obj: src/%
	${CC} -c -MMD -MP -o $@ $< ${CCOPTS}
	
# linker
${OUTS}: $(OBJS)
	${CC} -o bin/$@ b/$@.cpp.obj  $(filter-out $(OUTS:%=b/%.cpp.obj),$(OBJS)) ${LLOPTS}


clean:
	rm -rf b/* bin/*
	

# test
#TEST_D:=$(shell g++ -MM src/test.cpp -std=c++11 | ./deps.py)
#TEST_D:=$(TEST_D:src/%.RRR=b/%.cpp.obj)

#test: ${TEST_D}
#	${CC} -o $@ $^ ${LLOPTS}
	
# main
#MAIN_D:=$(shell g++ -MM src/main.cpp -std=c++11 | ./deps.py)
#MAIN_D:=$(MAIN_D:src/%.RRR=b/%.cpp.obj)

#main: ${MAIN_D}
#	${CC} -o $@ $^ ${LLOPTS}
	
	
