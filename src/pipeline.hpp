#ifndef TICKSTREAM_ENGINE_HPP
#define TICKSTREAM_ENGINE_HPP

#include <chrono>
#include <queue>
#include <vector>

#include "src/mergeBuffer.hpp"
#include "src/fileManager.hpp"

namespace tickstream {

inline bool comp(MarketData &a, MarketData &b) {
	return a.getTimeStamp() < b.getTimeStamp();
}
using MinHeap = std::priority_queue<MarketData, std::vector<MarketData>, decltype(&comp)>;

class Pipeline {
public:
    Pipeline(const char *inputDir, const char *outputDir, size_t batchSize, size_t totalBatches);

    void init();
    void run();
    void shutdown();

private:
	// Batch processing stage
	void processBatches();
	void processBatch(size_t index);
	std::vector<MarketData> loadBatch(const std::vector<std::string>& files);
	void sortBatch(std::vector<MarketData>& buffer);
	void writeBatch(size_t index, const std::vector<MarketData> &buffer);

	// K-way merge
	void mergeBatches();
	void initialiseMinHeap(MinHeap &minHeap);
	void initialiseMergeBuffer(std::vector<MergeBuffer> &mergeBuffer);
	void mergeRecords(MinHeap &minHeap);

	// Helper methods
	std::string getIntermediateFilePath(size_t index);
	std::string getOutputFilePath();
	void logTime(const std::string& label, std::chrono::time_point<std::chrono::high_resolution_clock> start);

	size_t _batchSize; // Number of files in a batch.
	size_t _totalBatches; // Number of total batches.
	const char *_inputDir;
	const char *_outputDir;

	FileManager _fileManager;
	std::vector<std::vector<std::string>> _batches;
};

} // namespace tickstream

#endif // TICKSTREAM_ENGINE_HPP
