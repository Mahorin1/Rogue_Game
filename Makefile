EXE_NAME := RogueMatraque

COMMON_LIBS := -lopengl32 -lwinmm -lgdi32 -lfreetype -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lpcg
LIBS := -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lenkiTS -lbox2 $(COMMON_LIBS)
DEBUG_LIBS := -lsfml-audio-s-d -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d -lenkiTS-d -lbox2d-d $(COMMON_LIBS)

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
COMPILE_FLAGS =

SRC := $(call rwildcard, src, *.cpp)
OBJ := $(patsubst src/%.cpp, gcc/obj/%.o, $(SRC))

REL_FLAGS := -O3 -DNDEBUG
DBG_FLAGS := -g -O0

.PHONY: all r d link_rel link_dbg

all: d

r: link_rel
	.\$(EXE_NAME).exe
d: link_dbg
	.\$(EXE_NAME).exe

link_rel: COMPILE_FLAGS = $(REL_FLAGS)
link_rel: $(OBJ) gcc/RC.res
	g++ $^ -o $(EXE_NAME) -L"gcc/lib" $(LIBS) -mwindows -O3

link_dbg: COMPILE_FLAGS = $(DBG_FLAGS)
link_dbg : $(OBJ) gcc/RC.res
	g++ $^ -o $(EXE_NAME) -L"gcc/lib" $(DEBUG_LIBS)

gcc/obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ -c $< -o $@ -I"include" -DSFML_STATIC $(COMPILE_FLAGS)

gcc/RC.res: gcc/RC.rc
	windres gcc/RC.rc -O coff -o gcc/RC.res

.PHONY: clean cleanO cleanE cleanRC

clean: cleanO cleanE

cleanO:
	rm -rf gcc/obj/*
cleanE:
	rm -f *.exe
cleanRC:
	rm -f gcc/RC.res