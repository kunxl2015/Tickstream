#include <filesystem>
#include <iostream>
#include <sstream>

#include "parser.h"
#include "marketData.hpp"

namespace tickstream {

FileManager::FileManager(const char *inputDir, const char *outputDir) :
	_inputDir(inputDir), _outputDir(outputDir) {}

void FileManager::readRecord(const size_t &fIndex, MarketData &mdata) {
	if (fIndex >= _inputStreams.size()) return;

	std::string line;
	std::getline(_inputStreams[fIndex], line);

	std::string buffer;
	std::getline(_inputStreams[fIndex], buffer);

	mdata.init(buffer);
}

void FileManager::openFiles() {
	namespace fs = std::filesystem;

	for (auto &entry: fs::directory_iterator(_inputDir)) {
		if (entry.path().extension() == ".txt") {
			if (!_inputStreams.emplace_back(entry.path(), std::ios::binary)) {
				std::cout << "Error in loading file: " << entry.path() << std::endl;
			} else {
				std::cout << "Opening file: " << entry.path() << std::endl;
			}
		}
	}
}

void FileManager::closeFiles() {
	for (auto &stream: _inputStreams) {
		if (stream.is_open()) stream.close();
	}

	if (_outputStream.is_open()) _outputStream.close();
}

} // namespace tickstream
