#ifndef TICKSTREAM_MARKET_DATA_H
#define TICKSTREAM_MARKET_DATA_H

#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace tickstream {

class MarketData {
public:
	MarketData() = default;

	MarketData(const char *timestamp, float price, uint32_t size,
			const char *exchange, const char *type):
		_price(price), _size(size), _timestamp(parseTimeStamp(timestamp)),
		_exchange(exchange), _type(type) {
	}

	// Initialise marketData object using a single string
	void init(const std::string &line, const size_t &fIndex) {
		std::stringstream ss(line);

		std::string buffer;
		std::getline(ss, buffer, ',');
		_timestamp = parseTimeStamp(buffer.c_str());

		ss >> _price;
		ss.ignore();

		ss >> _size;
		ss.ignore();

		std::getline(ss, buffer, ',');
		_exchange = strdup(buffer.c_str());

		std::getline(ss, buffer, ',');
		_type = strdup(buffer.c_str());

		_fIndex = fIndex;
	}

	bool operator<(const MarketData &other) const {
		return _timestamp < other._timestamp;
	}

	uint64_t getTimeStamp() const {
		return _timestamp;
	}

	const char *serialise() {
		const char *timestamp = formatTimeStamp(_timestamp);
		size_t size = std::snprintf(nullptr, 0, "%s, %.2f, %d,%s,%s\n",
				timestamp, _price, _size, _exchange, _type);

		char *buffer = new char[size + 1];
		std::snprintf(buffer, size + 1, "%s, %.2f, %d,%s,%s\n",
				timestamp, _price,	_size, _exchange, _type);

		return buffer;
	}

	const char *formatTimeStamp(uint64_t ts) {
		std::time_t epoch_seconds = ts / 1'000'000ULL;
		uint64_t microseconds = ts % 1'000'000ULL;

		std::tm *time = std::localtime(&epoch_seconds);

		time->tm_year += 1900;
		time->tm_mon += 1;

		thread_local char buffer[32];
		std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d.%03llu",
				time->tm_year, time->tm_mon, time->tm_mday,
				time->tm_hour, time->tm_min, time->tm_sec, microseconds);

		return buffer;
	}

	uint64_t parseTimeStamp(const char *ts) {
		std::tm time = {};
		uint64_t microseconds = 0;

		sscanf(ts, "%d-%d-%d %d:%d:%d.%llu",
				&time.tm_year, &time.tm_mon, &time.tm_mday,
				&time.tm_hour, &time.tm_min, &time.tm_sec, &microseconds);

		time.tm_year -= 1900;
		time.tm_mon -= 1;

		std::time_t epoch_seconds = std::mktime(&time);
		return epoch_seconds * 1'000'000ULL + microseconds;
	}

	size_t getFindex() {
		return _fIndex;
	}

	void print() {
		const char *buffer = serialise();
		std::cout << buffer;
	}
private:
	uint64_t _timestamp;
	const char *_exchange;
	const char *_type;
	float _price;
	size_t _fIndex = 0;
	uint32_t _size;
};

} // namespace tickstream

#endif // TICKSTREAM_MARKET_DATA_H

