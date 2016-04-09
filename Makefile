# Automatic Makefile E 15-10-23
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

# targets
debug: client/main server/test client/test
release: client/main server/test client/test

# em opts
EMOPTS:= 
EMOPTS+=-s USE_SDL=2 
EMOPTS+=-s DISABLE_EXCEPTION_CATCHING=0 
#EMOPTS+=-s SAFE_HEAP=1 
EMOPTS+=-s ASSERTIONS=1 
#EMOPTS+=-s DEMANGLE_SUPPORT=1
#EMOPTS+=-s EXCEPTION_DEBUG=1 

# compiler options
CCOPTS:=
CCOPTS+=-std=c++11
CCOPTS+=-I./inc -I./src -I./src/server
CCOPTS+=-Wsign-compare -Wreturn-type -Wparentheses -Wpedantic -Wconversion-null -Wno-vla-extension
#CCOPTS+=-ferror-limit=3



# linker options
LLOPTS:=


# emcc
ifeq (${CC}, emcc)	
	CCOPTS+=${EMOPTS}
	LLOPTS+=${EMOPTS}
	OUT_EXT:=.html	
	#LLOPTS+=--preload-file res	
else
	LLOPTS+=-L./lib 
	LLOPTS+=-lSDL2
	LLOPTS+=-lGL -lGLEW 
	OUT_EXT:=
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
	




