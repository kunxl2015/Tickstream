#include <vector>

#include "benchmark/benchmark.h"

static void BM_VectorPushBack(benchmark::State& state) {
	std::vector<int> v;
	for (auto _ : state) {
		v.push_back(42);
	}
}
BENCHMARK(BM_VectorPushBack);

BENCHMARK_MAIN();
