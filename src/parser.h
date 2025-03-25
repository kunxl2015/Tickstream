#ifndef TICKSTREAM_PARSER_H
#define TICKSTREAM_PARSER_H

#include "marketData.hpp"
#include <fstream>

namespace tickstream {

class FileManager{
public:
	FileManager() = default;
	~FileManager() = default;

	void readRecord(const size_t &fIndex, MarketData &mdata);

	void openFile(const char *fPath);
	void closeFiles();
private:
	std::ofstream _outputStream;
	std::vector<std::ifstream> _inputStreams;
};

} // namespace tickstream

#endif // TICKSTREAM_PARSER_H
