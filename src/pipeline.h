#ifndef TICKSTREAM_ENGINE_HPP
#define TICKSTREAM_ENGINE_HPP

#include "fileManager.h"
namespace tickstream {

class Pipeline {
public:
    Pipeline(const char *inputDir, const char *outputDir);

    void init();
    void run();
    void shutdown();

private:
	uint32_t _batchSize;
	const char *_inputDir;
	const char *_outputDir;

	FileManager _fileManger;
	std::vector<std::vector<std::string>> _batches;
};

extern Pipeline app;

} // namespace tickstream

#endif // TICKSTREAM_ENGINE_HPP
