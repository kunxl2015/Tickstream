#include "src/mergeBuffer.hpp"
#include "src/pipeline.hpp"

namespace tickstream {

void Pipeline::processBatches() {
	for (size_t batchIndex = 0; batchIndex < _totalBatches; batchIndex++) {
		if (!_batches[batchIndex].size())
			return;

		printf("[INFO] Handling Batch %zu\n", batchIndex);
		auto batchStart = std::chrono::high_resolution_clock::now();
		processBatch(batchIndex);
		logTime("[TIME] Batch Processing Time (index: " + std::to_string(batchIndex) + ")", batchStart);
	}
}

void Pipeline::processBatch(size_t index) {
	auto batchStart = std::chrono::high_resolution_clock::now();
	std::vector<MarketData> buffer = loadBatch(_batches[index]);
	logTime("[TIME] Load Batch Time (index: " + std::to_string(index) + ")", batchStart);

	auto sortStart = std::chrono::high_resolution_clock::now();
	sort(buffer.begin(), buffer.end());
	logTime("[TIME] Sort Batch Time (index: " + std::to_string(index) + ")", sortStart);

	auto writeStart = std::chrono::high_resolution_clock::now();
	writeBatch(index, buffer);
	logTime("[TIME] Write Batch Time (index: " + std::to_string(index) + ")", writeStart);
}

std::vector<MarketData> Pipeline::loadBatch(const std::vector<std::string>& files) {
	std::vector<MarketData> buffer;
	buffer.reserve(files.size() * 200000);

	for (size_t i = 0; i < files.size(); i++) {
		printf("[INFO] Loading File: %s\n", files[i].c_str());
		_fileManager.openFile(files[i].c_str());
		_fileManager.prepareFile(i);
		_fileManager.readRecords(i, buffer);
		printf("[INFO] Records Loaded from File: %s\n", files[i].c_str());
	}
	return buffer;
}

void Pipeline::sortBatch(std::vector<MarketData>& buffer) {
	printf("[INFO] Sorting Batch with %zu records\n", buffer.size());
	std::sort(buffer.begin(), buffer.end());
}

void Pipeline::writeBatch(size_t index, const std::vector<MarketData>& buffer) {
	if (buffer.empty()) return;

	printf("[INFO] Writing Batch %zu with %zu records\n", index, buffer.size());
	std::string outputPath = getIntermediateFilePath(index);
	_fileManager.setOutputStream(outputPath.c_str());
	_fileManager.writeRecords(buffer);
	_fileManager.closeFiles();
}

void Pipeline::mergeBatches() {
	auto mergeStart = std::chrono::high_resolution_clock::now();

	MinHeap minHeap(comp);
	initialiseMinHeap(minHeap);

	std::vector<MergeBuffer> mergeBuffer;
	mergeBuffer.resize(_totalBatches);
	initialiseMergeBuffer(mergeBuffer);

	mergeRecords(minHeap, mergeBuffer);

	logTime("[TIME] Total Merge Time", mergeStart);
}

void Pipeline::initialiseMergeBuffer(std::vector<MergeBuffer> &mergeBuffer) {
	for (size_t fIndex = 0; fIndex < _totalBatches; fIndex++) {
		mergeBuffer[fIndex].setFileIndex(fIndex);
		mergeBuffer[fIndex].refill(_fileManager);
		mergeBuffer.emplace_back(mergeBuffer[fIndex]);
	}
	printf("[INFO] Initialized MergeBuffer of size: %zu\n", mergeBuffer[0]._records.size());
}

void Pipeline::initialiseMinHeap(MinHeap &minHeap) {
	auto start = std::chrono::high_resolution_clock::now();

	for (size_t fIndex = 0; fIndex < _totalBatches; fIndex++) {
		std::string intermediatePath = getIntermediateFilePath(fIndex);
		printf("Opening Intermediate Path: %s\n", intermediatePath.c_str());
		if (_fileManager.openFile(intermediatePath.c_str())) {
			MarketData mdata;
			if (_fileManager.readRecord(fIndex, mdata)) {
				minHeap.emplace(std::move(mdata));
			}
		}
	}

	printf("[INFO] Initialised MinHeap\n");
	logTime("[INFO] Initialising MinHeap Time: ", start);
}

void Pipeline::mergeRecords(MinHeap &minHeap, std::vector<MergeBuffer> &mergeBuffer) {
	printf("[INFO] Starting Merge Operation\n");

	std::string outputPath = getOutputFilePath();
	_fileManager.setOutputStream(outputPath.c_str());

	std::vector<MarketData> buffer;
	buffer.reserve(10000000);

	while (!minHeap.empty()) {
		MarketData current = std::move(minHeap.top());
		minHeap.pop();

		buffer.emplace_back(std::move(current));

		if (buffer.size() >= 10000000) {
			printf("[INFO] Writing the first 1000000\n");
			_fileManager.writeRecords(buffer);
			buffer.clear();
		}

		MarketData nextData;
		int currentIndex = current.getFindex();
		// Change this to take in data from the buffer
		if (mergeBuffer[currentIndex].empty()) {
			mergeBuffer[currentIndex].refill(_fileManager);
		}

		if (mergeBuffer[currentIndex].next(nextData)) {
			minHeap.push(std::move(nextData));
		}
	}

	_fileManager.writeRecords(buffer);
	_fileManager.closeFiles();
	printf("[INFO] Merge Operation Completed\n");
}

std::string Pipeline::getIntermediateFilePath(size_t index) {
	return "./intermediate/io" + std::to_string(index) + ".txt";
}

std::string Pipeline::getOutputFilePath() {
	return "./output/final_sorted.txt";
}

void Pipeline::logTime(const std::string& label, std::chrono::time_point<std::chrono::high_resolution_clock> start) {
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	printf("%s: %llu ms\n", label.c_str(), duration);
}

} // namespace tickstream
