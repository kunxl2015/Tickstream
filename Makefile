ROOT_DIR := $(shell pwd)
OS := $(shell uname)

BUILD_DIR := ./bin/
APP_NAME := build
BENCH_NAME := benchmarks

SRC_FILES := ./src/*.cpp
BENCH_FILES := ./benchmarks/*.cpp

CXXFLAGS := -std=c++20 -O3
DEBUGFLAGS := -DDEBUG
APP_INCLUDES := -I./includes/ -I./

INPUT_DIRECTORY ?= ./data/
OUTPUT_DIRECTORY ?= ./output/

ifeq ($(OS), Windows)
	CXX := g++ -O3
else
	CXX := clang++ -O3
endif

default: build run

build:
	$(CXX) $(CXXFLAGS) $(APP_INCLUDES) $(SRC_FILES) -o $(BUILD_DIR)$(APP_NAME)

debug:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(APP_INCLUDES) $(SRC_FILES) -o $(BUILD_DIR)$(APP_NAME)

benchmark:
	$(CXX) $(CXXFLAGS) -lbenchmark -lpthread $(APP_INCLUDES) $(BENCH_FILES) -o $(BUILD_DIR)$(BENCH_NAME)
run:
	$(BUILD_DIR)$(APP_NAME) $(INPUT_DIRECTORY) $(OUTPUT_DIRECTORY)
