#include <chrono>
#include <iostream>
#include "src/pipeline.hpp"

int main(int argc, char *argv[]) {
	/*
	 * Parse command-line arguments for the input directory (source data)
	 * and the output directory (destination of the sorted file).
	 */
	if (argc < 3) {
		std::cerr << "Directory paths are missing for source data and output direcotry. Usage: " << argv[0] << "<inputDir> <outputDir>\n";
		return 1;
	}

	const char *inputDir = argv[1];
	const char *outputDir = argv[2];
	size_t batchSize = (argc > 3) ? atoi(argv[3]) : 100;
	size_t totalFiles = (argc > 4) ? atoi(argv[4]) : 10000;

	std::cout << "Program name: " << argv[0] << std::endl;
	std::cout << "InputDir: " << inputDir << std::endl;
	std::cout << "OutputDir: " << outputDir << std::endl;

	// Initialise and Start the Pipeline.
	auto start = std::chrono::high_resolution_clock::now();

	tickstream::Pipeline pipeline(inputDir, outputDir, batchSize, (totalFiles / batchSize) + 1);
	pipeline.init();
	pipeline.run();
	pipeline.shutdown();

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	printf("Time taken for execution is: %lld", duration.count());

	return 0;
}
