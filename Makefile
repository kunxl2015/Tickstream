ROOT_DIR := $(shell pwd)
OS := $(shell uname)

BUILD_DIR := ./bin/
APP_NAME := build
BENCH_NAME := benchmarks

SRC_FILES := ./src/*.cpp
BENCH_FILES := ./benchmarks/*.cpp

APP_INCLUDES := -I./includes/
CXXFLAGS := -std=c++20

ifeq ($(OS), Windows)
	CXX := g++ -O3
else
	CXX := clang++ -O3
endif

default: build run

build:
	$(CXX) $(SRC_FILES) -o $(BUILD_DIR)$(APP_NAME) $(CXXFLAGS)

debug:
	$(CXX) main.cpp -o $(BUILD_DIR)$(APP_NAME) $(CXXFLAGS)

benchmark:
	$(CXX) $(BENCH_FILES) -o $(BUILD_DIR)$(BENCH_NAME) $(CXXFLAGS) -lbenchmark -lpthread

run:
	$(BUILD_DIR)$(APP_NAME) ./ ./
