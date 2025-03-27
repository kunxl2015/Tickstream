#include "src/pipeline.hpp"
#include <cstdint>

namespace tickstream {

void Pipeline::processBatches() {
	for (size_t batchIndex = 0; batchIndex < _batches.size(); batchIndex++) {
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
	printf("[INFO] Starting Merge Operation\n");
	auto mergeStart = std::chrono::high_resolution_clock::now();

	MinHeap minHeap;
	initializeMinHeap(minHeap);
	mergeRecords(minHeap);

	logTime("[TIME] Total Merge Time", mergeStart);
}

void Pipeline::initializeMinHeap(MinHeap &minHeap) {
	for (size_t fIndex = 0; fIndex < 100; fIndex++) {
		std::string intermediatePath = getIntermediateFilePath(fIndex);
		if (_fileManager.openFile(intermediatePath.c_str())) {
			MarketData mdata;
			if (_fileManager.readRecord(fIndex, mdata)) {
				minHeap.push(std::move(mdata));
				printf("[INFO] Initialized MinHeap with File Index %zu\n", fIndex);
			}
		}
	}
}

void Pipeline::mergeRecords(MinHeap &minHeap) {
	std::string outputPath = getOutputFilePath();
	_fileManager.setOutputStream(outputPath.c_str());

	std::vector<MarketData> buffer;
	buffer.reserve(1000);

	while (!minHeap.empty()) {
		MarketData current = std::move(minHeap.top());
		minHeap.pop();

		buffer.emplace_back(std::move(current));

		if (buffer.size() >= 1000) {
			_fileManager.writeRecords(buffer);
			buffer.clear();
		}

		MarketData nextData;
		int currentIndex = current.getFindex();
		if (_fileManager.readRecord(currentIndex, nextData)) {
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
