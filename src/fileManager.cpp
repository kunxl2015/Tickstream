#include "src/fileManager.hpp"
#include "src/marketData.hpp"

#include <filesystem>

namespace tickstream {

FileManager::FileManager() {
	_filenames.reserve(100);
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

	printf("Opening File: %s from: %lu files\n", _filenames[fIndex].c_str(), _filenames.size());
	mdata.init(line, fIndex, "");
	return true;
}

// TODO: Optimise sys-calls over here.
void FileManager::readRecords(const size_t &fIndex, std::vector<MarketData> &buffer) {
	if (fIndex >= _inputStreams.size()) return;

	const size_t BUFFER_SIZE = 10 * 1024 * 1024; // 4 MB
	std::vector<char> chunk(BUFFER_SIZE);
	std::string lineBuffer;

	while (_inputStreams[fIndex]) {
		_inputStreams[fIndex].read(chunk.data(), BUFFER_SIZE);
		std::streamsize bytesRead = _inputStreams[fIndex].gcount();

		char *start = chunk.data();
		char *end = chunk.data() + bytesRead;

		while (start < end) {
			char *newline = static_cast<char *>(memchr(start, '\n', end - start));

			if (!newline) {
				lineBuffer.append(start, end - start);
				break;
			}

			lineBuffer.append(start, newline - start);

			MarketData mdata;
			mdata.init(lineBuffer, fIndex, _filenames[fIndex].c_str());
			buffer.emplace_back(std::move(mdata));

			lineBuffer.clear();
			start = newline + 1;
		}
	}

	if (!lineBuffer.empty()) {
		MarketData mdata;
		mdata.init(lineBuffer, fIndex, _filenames[fIndex].c_str());
		buffer.emplace_back(std::move(mdata));
	}
}

void FileManager::writeRecords(const std::vector<MarketData> &buffer) {
	std::vector<char> writeBuffer;
	writeBuffer.reserve(buffer.size() * 128);

	for (const MarketData &item: buffer) {
		const char *data = item.serialise();
		size_t len = std::strlen(data);

		writeBuffer.insert(writeBuffer.end(), data, data + len);
		delete[] data;
	}

	_outputStream.write(writeBuffer.data(), writeBuffer.size());
}

bool FileManager::openFile(const char *fPath) {
	if (!_inputStreams.emplace_back(fPath, std::ios::binary)) {
		return false;
	}

	std::string name = std::filesystem::path(fPath).stem().string();
	_filenames.emplace_back(name.c_str());

	return true;
}

void FileManager::closeFiles() {
	for (auto &stream: _inputStreams) {
		if (stream.is_open()) stream.close();
	}

	if (_outputStream.is_open()) _outputStream.close();

	_filenames.clear();
	_inputStreams.clear();
	_outputStream.clear();
}

void FileManager::setOutputStream(const char *fPath) {
	if (_outputStream.is_open())
		_outputStream.close();

	namespace fs = std::filesystem;

	fs::path filePath(fPath);
	fs::path dirPath = filePath.parent_path();

	if (!dirPath.empty() && !fs::exists(dirPath)) {
		if (!fs::create_directories(dirPath)) {
			std::cerr << "Failed to create directory: " << dirPath << std::endl;
			return;
		}
	}

	_outputStream.open(fPath, std::ios::out | std::ios::binary);
	if (!_outputStream) {
		std::cerr << "Unable to open file: " << fPath << std::endl;
		return;
	}
}

} // namespace tickstream
