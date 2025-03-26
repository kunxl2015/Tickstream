#ifndef TICKSTREAM_PARSER_H
#define TICKSTREAM_PARSER_H

#include <fstream>

#include "src/marketData.h"

namespace tickstream {

class FileManager{
public:
	FileManager();
	~FileManager() = default;

	void prepareFile(const size_t fIndex);

	bool readRecord(const size_t &fIndex, MarketData &mdata);
	void writeRecord(MarketData &mdata);
	void readRecords(const size_t &fIndex, MarketData &mdata, std::vector<MarketData> &buffer);
	void writeRecords(MarketData buffer[], int bufferSize);

	void closeFiles();
	bool openFile(const char *fPath);
	void setOutputStream(const char *fPath);
private:
	std::ofstream _outputStream;
	std::vector<std::ifstream> _inputStreams;
};

} // namespace tickstream

#endif // TICKSTREAM_PARSER_H
