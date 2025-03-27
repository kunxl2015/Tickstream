#include <iostream>
#include <ostream>
#include <queue>
#include <string>
#include <vector>

#include "src/pipeline.hpp"

namespace tickstream {

Pipeline::Pipeline(const char *inputDir, const char *outputDir) {
	_batchSize = 100;
	_totalBatches = 100;
	_inputDir = inputDir;
	_outputDir = outputDir;
}

void Pipeline::init() {
	namespace fs = std::filesystem;

	_batches.resize(_totalBatches);
	for (int i = 0; i < _totalBatches; i++) {
		_batches[i].reserve(_batchSize);
	}

	int fileCount = 0;
	for (auto &entry: fs::directory_iterator(_inputDir)) {
		if (entry.path().extension() == ".txt") {
			int index = fileCount / _batchSize;

			if (index >= _totalBatches) {
				_totalBatches += 1;
				_batches.emplace_back();
			}

			_batches[index].emplace_back(entry.path());
			fileCount++;
		}
	}
}

void Pipeline::run() {
	/*
	 * Process all batches:
	 * 1. Opens and reads market data files for each batch.
	 * 2. Sorts the market data within each batch in-memory.
	 * 3. Writes the sorted batch to an intermediate output file.
	 */
	processBatches();

	/*
	 * Performs a k-way merge on the sorted intermediate files:
	 * 1. Uses a min-heap to merge data from all intermediate files.
	 * 2. Outputs the globally sorted data to a final output file.
	 */
	mergeBatches();
}

void Pipeline::shutdown() {
	_fileManager.closeFiles();
}

} // namespace tickstream
