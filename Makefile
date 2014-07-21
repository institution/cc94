# $@ -- The file name of the target of the rule.
# $* -- The stem with which an implicit rule matches
# $< -- The name of the first prerequisite.
# $^ -- The names of all the prerequisites
# $(VARS:%.cpp=%.o) -- pattern replace

# output files
OUTS:=main test

# temporary dont build following files
IGNORE_SRC:=src/tree.cpp

WARNOPTS:=-Wsign-compare -Wreturn-type -Wparentheses -Wpedantic
# -Wall -Wextra 

DEBUGOPTS:=-g -O0
PROFILEOPTS:=-O3 -pg

# compiler options
CCOPTS:=-I./src -I./inc -std=c++11 -fmax-errors=5 ${WARNOPTS} ${DEBUGOPTS}

# linker options
LLOPTS:=-lboost_serialization -lboost_program_options -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network ${DEBUGOPTS}


# assert dirs
$(shell mkdir -p b)
$(shell mkdir -p bin)

# list of compiled source b/fname.cpp.obj
OBJS:=$(shell find src -name '*.cpp')
OBJS:=$(filter-out $(IGNORE_SRC),$(OBJS))
OBJS:=$(OBJS:src/%.cpp=b/%.cpp.obj)

-include $(OBJS:%.obj=%.d)

# compiler
${OBJS}: b/%.obj: src/%
	g++ -c -MMD -MP -o $@ $< ${CCOPTS}
	
# linker
${OUTS}: $(OBJS)
	g++ -o bin/$@ $(filter-out b/$(filter-out $@,$(OUTS)).cpp.obj,$(OBJS)) ${LLOPTS}

clean:
	rm -f b/* bin/*
	

# test
#TEST_D:=$(shell g++ -MM src/test.cpp -std=c++11 | ./deps.py)
#TEST_D:=$(TEST_D:src/%.RRR=b/%.cpp.obj)

#test: ${TEST_D}
#	g++ -o $@ $^ ${LLOPTS}
	
# main
#MAIN_D:=$(shell g++ -MM src/main.cpp -std=c++11 | ./deps.py)
#MAIN_D:=$(MAIN_D:src/%.RRR=b/%.cpp.obj)

#main: ${MAIN_D}
#	g++ -o $@ $^ ${LLOPTS}
	
	
