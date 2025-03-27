#ifndef TICKSTREAM_MERGEBUFFER_HPP
#define TICKSTREAM_MERGEBUFFER_HPP

#include "src/fileManager.hpp"
#include "src/marketData.hpp"

namespace tickstream {

class MergeBuffer {
public:
	MergeBuffer() = default;
	MergeBuffer(size_t fileIndex) {
		_fileIndex = fileIndex;
	}

	bool empty() const {
		return _currentIndex >= _records.size();
	}

	bool next(MarketData &mdata) {
		if (_currentIndex >= _records.size())
			return false;

		mdata = std::move(_records[_currentIndex]);
		_currentIndex += 1;

		return true;
	}

	void setFileIndex(size_t fIndex) {
		_fileIndex = fIndex;
	}

	void refill(FileManager &fileManager) {
		_currentIndex = 0;
		_records.clear();
		fileManager.readRecords(_fileIndex, _records);
	}
public:
	size_t _fileIndex = 0;
	size_t _currentIndex = 0;
	std::vector<MarketData> _records;
};

} // namespace tickstream

#endif // TICKSTREAM_MERGEBUFFER_HPP
