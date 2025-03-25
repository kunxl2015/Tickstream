#ifndef TICKSTREAM_MARKET_DATA_H
#define TICKSTREAM_MARKET_DATA_H

#include <cstdint>
#include <ctime>
#include <iostream>

namespace tickstream {

class MarketData {
public:
	MarketData(float price, uint32_t size, const char *exchange, const char *type, const char *timestamp):
		_price(price), _size(size), _timestamp(parseTimeStamp(timestamp)),
		_exchange(exchange), _type(type) {}

	bool operator<(const MarketData &other) const {
		return _timestamp < other._timestamp;
	}

	uint64_t parseTimeStamp(const char *ts) {
		std::tm time = {};
		int microseconds = 0;

		sscanf(ts, "%d-%d-%d %d:%d:%d.%d",
				&time.tm_year, &time.tm_mon, &time.tm_mday,
				&time.tm_hour, &time.tm_min, &time.tm_sec, &microseconds);

		time.tm_year -= 2000;
		time.tm_mon -= 1;

		std::time_t epoch_seconds = std::mktime(&time);
		return epoch_seconds * 1'000'000ULL + microseconds;
	}

	void print() {
		std::cout << _timestamp << "," << _price << "," << _size << "," << _exchange << "," << _type << std::endl;
	}
private:
	uint64_t _timestamp;
	const char *_exchange;
	const char *_type;
	float _price;
	uint32_t _size;
};

} // namespace tickstream

#endif // TICKSTREAM_MARKET_DATA_H

