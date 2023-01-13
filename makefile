ifeq ($(COMP), clang)
CXX = clang++
else
CXX = g++
endif

ifeq ($(PROF), true)
CXXPROF += -pg -no-pie
else
CXXPROF += -s
endif

ifeq ($(BUILD), debug)
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g -march=native
else
CXXFLAGS += -DNDEBUG -std=c++20 -O3 $(CXXPROF) -march=native
endif

ifeq ($(USE_PEXT), true)
CXXFLAGS += -DUSE_PEXT
endif

STATIC_LIB = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -lsetupapi -lhid -static

.PHONY: all test clean makedir

all: test tuner

test: makedir
	@$(CXX) $(CXXFLAGS) "core\*.cpp" "test\*.cpp" $(STATIC_LIB) -o "bin\test\test.exe"

clean: makedir
	@echo Cleaning the bin directory
	@rm -rf bin
	@make makedir

makedir:
	@mkdir -p bin
	@mkdir -p bin\test

.DEFAULT_GOAL := test