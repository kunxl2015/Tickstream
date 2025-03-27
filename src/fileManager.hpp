#ifndef TICKSTREAM_PARSER_H
#define TICKSTREAM_PARSER_H

#include "src/pch.hpp"
#include "src/marketData.hpp"

namespace tickstream {

class FileManager{
public:
	FileManager();
	~FileManager() = default;

	void prepareFile(const size_t fIndex);

	bool readRecord(const size_t &fIndex, MarketData &mdata);
	void readRecords(const size_t &fIndex, std::vector<MarketData> &buffer);
	bool writeRecord(MarketData &mdata);
	void writeRecords(const std::vector<MarketData> &buffer);

	void closeFiles();
	bool openFile(const char *fPath);
	void setOutputStream(const char *fPath);
private:
	std::vector<std::string> _filenames;

	std::ofstream _outputStream;
	std::vector<std::ifstream> _inputStreams;
};

} // namespace tickstream

#endif // TICKSTREAM_PARSER_H
