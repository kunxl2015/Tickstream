#include "pipeline.h"
#include <ostream>

namespace tickstream {

Pipeline::Pipeline(const char *inputDir, const char *outputDir) {
	_batchSize = 100;
	_inputDir = inputDir;
	_outputDir = outputDir;
}

void Pipeline::init() {
	namespace fs = std::filesystem;

	_batches.resize(100);
	for (int i = 0; i < 100; i++) {
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
	// TODO: Optimise file opening & copying the buffer
	// using multiple threads
	for (size_t batchIndex = 0; batchIndex < _batches.size(); batchIndex++) {
		std::vector<std::string> batch = _batches[batchIndex];
		size_t batchSize = batch.size();

		std::vector<MarketData> buffer;
		buffer.reserve(batchSize);

		for (size_t i = 0; i < batchSize; i++) {
			MarketData mdata;
			_fileManger.openFile(batch[i].c_str());
			_fileManger.prepareFile(i);
			_fileManger.readRecord(i, mdata);
			_fileManger.readRecord(i, mdata);
			_fileManger.readRecord(i, mdata);
			_fileManger.readRecord(i, mdata);
			buffer.emplace_back(mdata);
		}

		// Sort The buffer
		sort(buffer.begin(), buffer.end());

		// Write to buffer
		std::string outputPath = "./output/io" + std::to_string(batchIndex) + ".txt";
		_fileManger.setOutputStream(outputPath.c_str());
		_fileManger.writeRecord(buffer.data(), buffer.size());
	}

	// Sort Each Batch and print to intermediate file

	// K-way merge for the final 100 files
}

void Pipeline::shutdown() {

}

} // namespace tickstream
