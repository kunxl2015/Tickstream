#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "src/fileManager.h"
#include "src/marketData.h"

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

bool FileManager::readRecord(const size_t &fIndex, MarketData &mdata) {
	if (fIndex >= _inputStreams.size())
		return false;

	if (!_inputStreams[fIndex].is_open() || _inputStreams[fIndex].eof())
		return false;

	std::string line;
	std::getline(_inputStreams[fIndex], line);
	if (line.size() == 0) return false;

	mdata.init(line, fIndex);
	return true;
}

// Optimise how you can minimise the number of sys-calls over here.
void FileManager::readRecords(const size_t &fIndex, MarketData &mdata, std::vector<MarketData> &buffer) {
	if (fIndex >= _inputStreams.size()) return;

	std::string line;
	while (std::getline(_inputStreams[fIndex], line)) {
		mdata.init(line, fIndex);
		buffer.emplace_back(mdata);
	}
}

void FileManager::writeRecords(MarketData buffer[], int bufferSize) {
	for (int i = 0; i < bufferSize; i++) {
		const char *data = buffer[i].serialise();
		_outputStream.write(data, strlen(data));
	}
}

bool FileManager::openFile(const char *fPath) {
	if (!_inputStreams.emplace_back(fPath, std::ios::binary)) {
		return false;
	}

	return true;
}

void FileManager::closeFiles() {
	for (auto &stream: _inputStreams) {
		if (stream.is_open()) stream.close();
	}

	if (_outputStream.is_open()) _outputStream.close();
	_inputStreams.clear();
	_outputStream.clear();
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
