#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "fileManager.h"
#include "marketData.hpp"

namespace tickstream {

FileManager::FileManager() {
	_inputStreams.reserve(100);
}

void FileManager::prepareFile(const size_t fIndex) {
	if (fIndex >= _inputStreams.size()) return;

	std::string line;
	std::getline(_inputStreams[fIndex], line);

	return;
}
// Optimise how you can minimise the number of sys-calls over here.
void FileManager::readRecord(const size_t &fIndex, MarketData &mdata) {
	if (fIndex >= _inputStreams.size()) return;

	std::string buffer;
	std::getline(_inputStreams[fIndex], buffer);

	mdata.init(buffer);
}

void FileManager::writeRecord(MarketData buffer[], int bufferSize) {
	for (int i = 0; i < bufferSize; i++) {
		const char *data = buffer[i].serialise();
		_outputStream.write(data, strlen(data));
	}
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

void FileManager::setOutputStream(const char *fPath) {
	if (_outputStream.is_open())
		_outputStream.close();

	_outputStream.open(fPath, std::ios::out);
	if (!_outputStream) {
		std::cerr << "Unable to open file: " << fPath << std::endl;
		return;
	}
}

} // namespace tickstream
