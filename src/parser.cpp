#include <filesystem>
#include <iostream>
#include <sstream>

#include "parser.h"
#include "marketData.hpp"

namespace tickstream {

void FileManager::readRecord(const size_t &fIndex, MarketData &mdata) {
	if (fIndex >= _inputStreams.size()) return;

	std::string line;
	std::getline(_inputStreams[fIndex], line);

	std::string buffer;
	std::getline(_inputStreams[fIndex], buffer);

	mdata.init(buffer);
}

void FileManager::openFile(const char *fPath) {
	if (!_inputStreams.emplace_back(fPath, std::ios::binary)) {
		std::cerr << "Error in opening file: " << fPath << std::endl;
	}
}

void FileManager::closeFiles() {
	for (auto &stream: _inputStreams) {
		if (stream.is_open()) stream.close();
	}

	if (_outputStream.is_open()) _outputStream.close();
}

} // namespace tickstream
