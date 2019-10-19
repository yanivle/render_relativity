CC			= clang++
LD			= clang++
RM			= rm -rf
OPEN		= open

OBJDIR		= obj
BINDIR		= bin

CPPFLAGS	:= -std=c++17 -O3 -g -D_GLIBCXX_DEBUG -c
LDFLAGS		:= -pthread -lc

SRC			:= $(wildcard ./*.cc)
OBJ			:= $(patsubst ./%,$(OBJDIR)/%,$(SRC:.cc=.o))
HDR			:= $(wildcard ./*.h)
TEST_SRC			:= $(wildcard ./tests/*.cc)
TEST_OBJ			:= $(patsubst ./%,$(OBJDIR)/%,$(TEST_SRC:.cc=.o))

# Verbosity Control, ala automake
V 			= 1

# Verbosity for CC
REAL_CC 	:= $(CC)
CC_0 		= @echo "CC $<"; $(REAL_CC)
CC_1 		= $(REAL_CC)
CC 			= $(CC_$(V))

# Verbosity for LD
REAL_LD 	:= $(LD)
LD_0 		= @echo "LD $@"; $(REAL_LD)
LD_1 		= $(REAL_LD)
LD 			= $(LD_$(V))

# Verbosity for RM
REAL_RM 	:= $(RM)
RM_0 		= @echo "Cleaning..."; $(REAL_RM)
RM_1 		= $(REAL_RM)
RM 			= $(RM_$(V))

# Verbosity for RMDIR
REAL_RMDIR 	:= $(RMDIR)
RMDIR_0 	= @$(REAL_RMDIR)
RMDIR_1 	= $(REAL_RMDIR)
RMDIR 		= $(RMDIR_$(V))

# Build Rules
.PHONY: clean
.DEFAULT_GOAL := all

all: setup render_relativity tests
setup: dir
remake: clean all

CREATE_DEPENDS:=$(shell ./create_dependencies.sh)
-include .depend

.PHONY: render_relativity
render_relativity: $(BINDIR)/render_relativity
.PHONY: tests
tests: $(BINDIR)/tests_main

$(BINDIR)/render_relativity: $(OBJDIR)/main.o $(OBJDIR)/counters.o $(OBJDIR)/material.o $(OBJDIR)/perlin_noise.o $(OBJDIR)/object_registry.o $(OBJDIR)/world_constants.o
	$(LD) $(LDFLAGS) $^ -o $@

$(BINDIR)/tests_main: $(TEST_OBJ) $(OBJDIR)/counters.o $(OBJDIR)/material.o $(OBJDIR)/perlin_noise.o $(OBJDIR)/object_registry.o $(OBJDIR)/world_constants.o
	$(LD) $(LDFLAGS) $^ -o $@

dir:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/tests
	@mkdir -p $(BINDIR)

clean:
	$(RM) $(OBJDIR) $(BINDIR)

movie: render_relativity
	rm -f output/*
	./bin/render_relativity
	python ./animation-generator.py
	$(OPEN) output/movie.mp4
