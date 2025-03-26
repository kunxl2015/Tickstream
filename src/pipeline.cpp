#include "pipeline.h"
#include <ostream>
#include <queue>

namespace tickstream {

Pipeline::Pipeline(const char *inputDir, const char *outputDir) {
	_batchSize = 100;
	_inputDir = inputDir;
	_outputDir = outputDir;
}

void Pipeline::init() {
	namespace fs = std::filesystem;

	_batches.resize(_batchSize);
	for (int i = 0; i < _batchSize; i++) {
		_batches[i].reserve(100);
	}

	int fileCount = 0;
	for (auto &entry: fs::directory_iterator(_inputDir)) {
		int index = fileCount / _batchSize;

		if (entry.path().extension() == ".txt"){
			_batches[index].emplace_back(entry.path().c_str());
			std::cout << entry.path().c_str() << std::endl;
		}
		fileCount++;
	}
}

void Pipeline::run() {
	// TODO: Optimise memory management &
	// sorting using the CPU
	for (size_t batchIndex = 0; batchIndex < _batches.size(); batchIndex++) {
		std::vector<std::string> batch = _batches[batchIndex];
		size_t batchSize = batch.size();

		std::vector<MarketData> buffer;
		buffer.reserve(batchSize);

		for (size_t i = 0; i < batchSize; i++) {
			MarketData mdata;
			_fileManager.openFile(batch[i].c_str());
			_fileManager.prepareFile(i);
			_fileManager.readRecords(i, mdata, buffer);
		}

		sort(buffer.begin(), buffer.end());

		if (buffer.size()) {
			std::string outputPath = "./intermediate/io" + std::to_string(batchIndex) + ".txt";
			_fileManager.setOutputStream(outputPath.c_str());
			_fileManager.writeRecords(buffer.data(), buffer.size());
			_fileManager.closeFiles();
		}

	}


	auto comp = [](const MarketData &a, const MarketData &b) {
		return a.getTimeStamp() > b.getTimeStamp();
	};
	std::priority_queue<MarketData, std::vector<MarketData>, decltype(comp)> minHeap;

	// Iterating through all the files
	for (size_t fIndex = 0; fIndex < 100; fIndex++) {
		std::string intermidatePath = "./intermediate/io" + std::to_string(fIndex) + ".txt";
		if (_fileManager.openFile(intermidatePath.c_str())) {
			MarketData mdata;
			_fileManager.readRecord(fIndex, mdata);
			minHeap.push(mdata);
		}
	}

	std::string outputPath = "./output/final_sorted.txt";
	_fileManager.setOutputStream(outputPath.c_str());
	std::vector<MarketData> buffer;
	buffer.reserve(1000);
	while (minHeap.size() > 0) {
		MarketData current = minHeap.top();
		minHeap.pop();

		buffer.emplace_back(current);
		if (buffer.size() >= 1000) {
			_fileManager.writeRecords(buffer.data(), buffer.size());
			buffer.clear();
		}

		std::cout << "Health check " << minHeap.size() << minHeap.empty() << std::endl;
		MarketData nextData;
		int currentIndex = current.getFindex();
		if (_fileManager.readRecord(currentIndex, nextData)) {
			nextData.print();
			minHeap.push(nextData);
		}
	}

	_fileManager.writeRecords(buffer.data(), buffer.size());
	_fileManager.closeFiles();

}

void Pipeline::shutdown() {
	_fileManager.closeFiles();
}

} // namespace tickstream
