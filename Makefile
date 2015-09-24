# Automatic Makefile 1.0.3
#    written by institution
#
# $@ -- The file name of the target of the rule.
# $* -- The stem with which an implicit rule matches
# $< -- The name of the first prerequisite.
# $^ -- The names of all the prerequisites
# $(VARS:%.cpp=%.o) -- pattern replace

#CC:=g++
CC:=clang++
#CC:=emcc

# output files
OUTS:=client/main server/test client/test

ifeq (${CC}, emcc)
	OUT_EXT:=.html
else
	OUT_EXT:=
endif

# targets
debug: client/main server/test client/test
release: client/main server/test client/test

COMMON_OPTS:= 
COMMON_OPTS+=-s USE_SDL=2 
COMMON_OPTS+=-s DISABLE_EXCEPTION_CATCHING=0 
#COMMON_OPTS+=-s SAFE_HEAP=1 
COMMON_OPTS+=-s ASSERTIONS=1 
#COMMON_OPTS+=-s DEMANGLE_SUPPORT=1

# compiler options
CCOPTS:=
CCOPTS+=-std=c++11
CCOPTS+=-I./inc -I./src -I./src/server
CCOPTS+=-Wsign-compare -Wreturn-type -Wparentheses -Wpedantic -Wconversion-null 
CCOPTS+=-ferror-limit=3
ifeq (${CC}, emcc)
	CCOPTS+=${COMMON_OPTS}
else
	
endif





# linker options
LLOPTS:=
ifeq (${CC}, emcc)	
	LLOPTS+=${COMMON_OPTS}
	#LLOPTS+=--preload-file res	
else
	LLOPTS+=-L./lib 
	LLOPTS+=-lSDL2 
endif

debug:   CCOPTS+=-O0 -g
debug:   LLOPTS+=-O0 -g
release: CCOPTS+=-O3 
release: LLOPTS+=-O3 






# assert dirs
$(shell mkdir -p b)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p b/{})
$(shell mkdir -p bin)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p bin/{})

# list of compiled source b/fname.cpp.obj
OBJS:=$(shell find src -name '*.cpp')
OBJS:=$(OBJS:src/%.cpp=b/%.cpp.obj)

-include $(OBJS:%.obj=%.d)


# compiler
${OBJS}: b/%.obj: src/%
	${CC} -c -MMD -MP -o $@ $< ${CCOPTS}
	
# linker
${OUTS}: $(OBJS)
	${CC} -o bin/$@${OUT_EXT} b/$@.cpp.obj  $(filter-out $(OUTS:%=b/%.cpp.obj),$(OBJS)) ${LLOPTS}
	


clean:
	rm -rf b/* bin/*
	




