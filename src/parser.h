#ifndef TICKSTREAM_PARSER_H
#define TICKSTREAM_PARSER_H

#include "marketData.hpp"
#include <fstream>

namespace tickstream {

class FileManager{
public:
	FileManager(const char *inputDir, const char *outputDir);
	~FileManager() = default;

	void readRecord(const size_t &fIndex, MarketData &mdata);

	void openFiles();
	void closeFiles();
private:
	const char *_inputDir;
	const char *_outputDir;

	std::ofstream _outputStream;
	std::vector<std::ifstream> _inputStreams;
};

} // namespace tickstream

#endif // TICKSTREAM_PARSER_H
