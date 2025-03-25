#ifndef TICKSTREAM_PARSER_H
#define TICKSTREAM_PARSER_H

#include "marketData.hpp"
#include <fstream>

namespace tickstream {

class FileManager{
public:
	FileManager();
	~FileManager() = default;

	void prepareFile(const size_t fIndex);
	void readRecord(const size_t &fIndex, MarketData &mdata);
	void writeRecord(MarketData buffer[], int bufferSize);

	void closeFiles();
	void openFile(const char *fPath);
	void setOutputStream(const char *fPath);
private:
	std::ofstream _outputStream;
	std::vector<std::ifstream> _inputStreams;
};

} // namespace tickstream

#endif // TICKSTREAM_PARSER_H
