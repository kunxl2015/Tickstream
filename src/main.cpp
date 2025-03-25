#include "pipeline.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << "<inputDir> <outputDir>\n";
		return 1;
	}

	const char *inputDir = argv[1];
	const char *outputDir = argv[2];

	std::cout << "Program name: " << argv[0] << std::endl;
	std::cout << "InputDir: " << inputDir << std::endl;
	std::cout << "OutputDir: " << outputDir << std::endl;

	// Initialise and start the pipeline
	tickstream::Pipeline pipeline(inputDir, outputDir);
	pipeline.init();
	pipeline.run();
	pipeline.shutdown();

	return 0;
}
