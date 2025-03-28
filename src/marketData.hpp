#ifndef TICKSTREAM_MARKET_DATA_H
#define TICKSTREAM_MARKET_DATA_H

#include <cstdint>
#include <string>

namespace tickstream {

class MarketData {
public:
	// Constructor
	MarketData();
	MarketData(const char *timestamp, float price, uint32_t size,
			   char *exchange, char *type);

	// Copy Constructor & Move Constructor
	MarketData(const MarketData &other);
	MarketData(MarketData &&other) noexcept;

	// Copy Assignment Operator & Move Assignment Operator
	MarketData &operator=(const MarketData &other);
	MarketData &operator=(MarketData &&other) noexcept;

	// Destructor
	~MarketData();

	bool operator<(const MarketData &other) const;

	void init(const std::string &line, const size_t &fIndex, const char *symbol);

	size_t getFindex() const;
	uint64_t getTimeStamp() const;

	void print();
	const char *serialise() const;

private:
	const char *formatTimeStamp(uint64_t ts) const;
	uint64_t parseTimeStamp(const char *ts) const;

	uint64_t _timestamp;
	char *_exchange;
	char *_symbol;
	char *_type;
	float _price;
	size_t _fIndex = 0;
	uint32_t _size;
};

} // namespace tickstream

#endif // TICKSTREAM_MARKET_DATA_H
