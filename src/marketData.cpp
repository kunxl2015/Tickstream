#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>

#include "src/marketData.hpp"

namespace tickstream {

MarketData::MarketData() {
	_exchange = nullptr;
	_symbol = nullptr;
	_type = nullptr;
	_timestamp = 0;
	_size = 0;
	_price = 0;
	_fIndex = 0;
}

// Copy Constructor (Deep Copy)
MarketData::MarketData(const MarketData &other) {
	_timestamp = other._timestamp;
	_price = other._price;
	_size = other._size;
	_fIndex = other._fIndex;

	if (other._exchange) {
		size_t len = std::strlen(other._exchange);
		_exchange = new char[len + 1];
		std::strcpy(_exchange, other._exchange);
	}

	if (other._symbol) {
		size_t len = std::strlen(other._symbol);
		_symbol = new char[len + 1];
		std::strcpy(_symbol, other._symbol);
	}

	if (other._type) {
		size_t len = std::strlen(other._type);
		_type = new char[len + 1];
		std::strcpy(_type, other._type);
	}
}

// Move Constructor (Steals Resources)
MarketData::MarketData(MarketData &&other) noexcept {
	_timestamp = other._timestamp;
	_price = other._price;
	_size = other._size;
	_exchange = other._exchange;
	_symbol = other._symbol;
	_type = other._type;
	_fIndex = other._fIndex;

	other._exchange = nullptr;
	other._symbol = nullptr;
	other._type = nullptr;
}

// Copy Assignment Operator
MarketData &MarketData::operator=(const MarketData &other) {
	if (this == &other) return *this;

	delete[] _exchange;
	delete[] _symbol;
	delete[] _type;

	_timestamp = other._timestamp;
	_price = other._price;
	_size = other._size;
	_fIndex = other._fIndex;

	if (other._exchange) {
		size_t len = std::strlen(other._exchange);
		_exchange = new char[len + 1];
		std::strcpy(_exchange, other._exchange);
	} else {
		_exchange = nullptr;
	}

	if (other._symbol) {
		size_t len = std::strlen(other._symbol);
		_symbol = new char[len + 1];
		std::strcpy(_symbol, other._symbol);
	} else {
		_symbol = nullptr;
	}

	if (other._type) {
		size_t len = std::strlen(other._type);
		_type = new char[len + 1];
		std::strcpy(_type, other._type);
	} else {
		_type = nullptr;
	}

	return *this;
}

// Move Assignment Opeartor
MarketData &MarketData::operator=(MarketData &&other) noexcept {
	if (this == &other) return *this;

	delete[] _exchange;
	delete[] _symbol;
	delete[] _type;

	_timestamp = other._timestamp;
	_price = other._price;
	_size = other._size;
	_fIndex = other._fIndex;
	_exchange = other._exchange;
	_symbol = other._symbol;
	_type = other._type;

	other._exchange = nullptr;
	other._symbol = nullptr;
	other._type = nullptr;

	return *this;
}

MarketData::~MarketData() {
	if (_exchange)
		delete[] _exchange;

	if (_symbol)
		delete[] _symbol;

	if (_type)
		delete[] _type;
}

// Initialise marketData object using a single string
void MarketData::init(const std::string &line, const size_t &fIndex, const char *symbol) {
	std::stringstream ss(line);

	std::string buffer;
	std::getline(ss, buffer, ',');
	_timestamp = parseTimeStamp(buffer.c_str());

	ss >> _price;
	ss.ignore();

	ss >> _size;
	ss.ignore();

	std::getline(ss, buffer, ',');
	_exchange = new char[buffer.size() + 1];
	std::strcpy(_exchange, (char *)buffer.c_str());

	std::getline(ss, buffer, ',');
	_type = new char[buffer.size() + 1];
	std::strcpy(_type, (char *)buffer.c_str());

	size_t len = std::strlen(symbol);
	_symbol = new char[len + 1];
	std::strcpy(_symbol, symbol);

	_fIndex = fIndex;
}

bool MarketData::operator<(const MarketData &other) const {
	if (_timestamp == other._timestamp) {
		return _symbol < other._symbol;
	}

	return _timestamp < other._timestamp;
}

uint64_t MarketData::getTimeStamp() const {
	return _timestamp;
}

size_t MarketData::getFindex() const {
	return _fIndex;
}

const char *MarketData::formatTimeStamp(uint64_t ts) const {
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

uint64_t MarketData::parseTimeStamp(const char *ts) const {
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

const char *MarketData::serialise() const {
	const char *timestamp = formatTimeStamp(_timestamp);
	size_t size = std::snprintf(nullptr, 0, "%s, %.2f, %d,%s,%s,%s\n",
			timestamp, _price, _size, _exchange, _type, _symbol);

	char *buffer = new char[size + 1];
	std::snprintf(buffer, size + 1, "%s, %.2f, %d,%s,%s,%s\n",
			timestamp, _price, _size, _exchange, _type, _symbol);

	return buffer;
}

void MarketData::print() {
	const char *buffer = serialise();
	std::cout << buffer;
	delete[] buffer;
}

} // namespace tickstream

