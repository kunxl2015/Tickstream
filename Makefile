ROOT_DIR := $(shell pwd)
OS := $(shell uname)

BUILD_DIR := ./bin/
APP_NAME := build
SRC_FILES := ./src/*.cpp

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

run:
	./main
