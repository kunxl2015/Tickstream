#ifndef TICKSTREAM_ENGINE_HPP
#define TICKSTREAM_ENGINE_HPP

#include "src/fileManager.h"

namespace tickstream {

class Pipeline {
public:
    Pipeline(const char *inputDir, const char *outputDir);

    void init();
    void run();
    void shutdown();

private:
	size_t _batchSize;
	size_t _totalBatches;
	const char *_inputDir;
	const char *_outputDir;

	FileManager _fileManager;
	std::vector<std::vector<std::string>> _batches;
};

extern Pipeline app;

} // namespace tickstream

#endif // TICKSTREAM_ENGINE_HPP
