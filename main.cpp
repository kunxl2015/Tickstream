#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/**
 * A simple MarketData structure.
 * Adjust fields/types as you need.
 */
#pragma pack(push, 1) // ensure consistent struct packing
struct MarketData
{
    uint32_t timestamp;          // e.g. nanoseconds from some epoch
    uint32_t token;              // symbol/token
    uint32_t bidPrice;           // best bid price
    uint32_t bidQuantity;        // best bid quantity
    uint32_t askPrice;           // best ask price
    uint32_t askQuantity;        // best ask quantity
    uint32_t lastTradedPrice;    // last traded price
    uint32_t lastTradedQuantity; // last traded quantity
};
#pragma pack(pop)

/**
 * A small helper structure for the priority queue.
 * It holds:
 *   - The current MarketData record.
 *   - Index of the file or a pointer to an ifstream from which this record came.
 */
struct DataPoint
{
    MarketData mktData;
    size_t fileIndex; // which file it came from

    // We want the smallest timestamp to come out of the priority queue first,
    // so define a custom comparator:
    bool operator>(const DataPoint &other) const
    {
        return mktData.timestamp > other.mktData.timestamp;
    }
};

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
    // 1) List all of the sorted binary input files you want to merge.
    //    Provide full or relative paths as necessary.
	std::vector<std::string> inputFiles = {
        "data_part1.txt",
        "data_part2.txt",
        "data_part3.txt"
    };

    // 2) Output file where we'll write merged data.
    string outputFile = "merged_output.bin";

    // Prepare ifstreams for each input file (in binary mode).
    vector<ifstream> streams;
    streams.reserve(inputFiles.size());
    for (auto &filePath : inputFiles)
    {
        streams.emplace_back(filePath, ios::binary);
        if (!streams.back().is_open())
        {
            cerr << "Error opening input file: " << filePath << endl;
            return 1;
        }
    }

    // Prepare ofstream for the merged output file.
    ofstream mergedOut(outputFile, ios::binary);
    if (!mergedOut.is_open())
    {
        cerr << "Error opening output file: " << outputFile << endl;
        return 1;
    }

    priority_queue<DataPoint, vector<DataPoint>, greater<DataPoint>> pq;
    auto readNextRecord = [&](size_t fileIdx, MarketData &mData) -> bool
    {
        if (!streams[fileIdx].read(reinterpret_cast<char*>(&mData), sizeof(MarketData)))
        {
            // If we can't read, it means either EOF or error.
            return false;
        }
        return true;
    };

    // 3) Initialize the priority queue by reading the first record from each file.
    for (size_t i = 0; i < streams.size(); i++)
    {
        // Try reading first record from file i
        MarketData md;
        if (readNextRecord(i, md))
        {
            DataPoint dp;
            dp.mktData = md;
            dp.fileIndex = i;
            pq.push(dp);
        }
    }

    // 4) Repeatedly pop the smallest-timestamp record from the queue,
    //    write it to the merged output, and then read the next record from that file.
    while (!pq.empty())
    {
        // Get the DataPoint with the smallest timestamp
        DataPoint current = pq.top();
        pq.pop();

        // Write it to the merged output file
        mergedOut.write(reinterpret_cast<const char*>(&current.mktData), sizeof(MarketData));

        // Read next record from the same file
        MarketData nextMd;
        if (readNextRecord(current.fileIndex, nextMd))
        {
            DataPoint newDp;
            newDp.mktData = nextMd;
            newDp.fileIndex = current.fileIndex;
            pq.push(newDp);
        }
    }

    // 5) Clean up
    mergedOut.close();
    for (auto &ifs : streams)
    {
        if (ifs.is_open())
            ifs.close();
    }

    cout << "Merging completed. Output written to: " << outputFile << endl;

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Time in ms:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    return 0;
}
