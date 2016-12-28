#include <cstddef>
#include <cassert>
#include <immintrin.h> // AVX2

#include <benchmark/benchmark.h>
#include <boost/numeric/ublas/vector.hpp>

namespace
{
  const std::size_t kVectorSize = 100000000;

  double doNaiveDotProduct(const std::vector<double>& v1, const std::vector<double>& v2)
  {
    assert((v1.size() == v2.size()) && "Dot product can only be done on vectors with the same size");

    double dotProduct = 0.0;

    for (std::size_t i = 0; i < v1.size(); ++i)
    {
      dotProduct += (v1[i] * v2[i]);
    }

    return dotProduct;
  }

  double doBoostDotProduct(const boost::numeric::ublas::vector<double>& v1, const boost::numeric::ublas::vector<double>& v2)
  {
    return inner_prod(v1, v2);
  }

  double doSimdDotProduct(const std::vector<double>& v1, const std::vector<double>& v2)
  {
    assert((v1.size() == v2.size()) && "Dot product can only be done on vectors with the same size");

    const std::size_t numPacks = v1.size() / 4; // 4 doubles per 256-bit register
    const std::size_t lastPackSize = v1.size() % 4;

    __m256d accumulated = _mm256_setzero_pd();
    __m256d a;
    __m256d b;

    for (std::size_t i = 0; i < numPacks; ++i)
    {
      a.m256d_f64[0] = v1[i * 4];
      a.m256d_f64[1] = v1[i * 4 + 1];
      a.m256d_f64[2] = v1[i * 4 + 2];
      a.m256d_f64[3] = v1[i * 4 + 3];

      b.m256d_f64[0] = v2[i * 4];
      b.m256d_f64[1] = v2[i * 4 + 1];
      b.m256d_f64[2] = v2[i * 4 + 2];
      b.m256d_f64[3] = v2[i * 4 + 3];

      accumulated = _mm256_fmaddsub_pd(a, b, accumulated);
    }

    const std::size_t lastPackOffset = numPacks * 4;

    for (std::size_t i = 0; i < 4; ++i)
    {
      a.m256d_f64[i] = (i < lastPackSize) ? v1[lastPackOffset + i] : 0.0;
      b.m256d_f64[i] = (i < lastPackSize) ? v2[lastPackOffset + i] : 0.0;
    }

    accumulated = _mm256_fmaddsub_pd(a, b, accumulated);

    __m128d accumulatedHigh = _mm256_extractf128_pd(accumulated, 1);
    __m128d sum = _mm_add_pd(accumulatedHigh, _mm256_castpd256_pd128(accumulated));

    return sum.m128d_f64[0] + sum.m128d_f64[1];
  }
}

static void naiveDotProduct(benchmark::State& state)
{
  std::vector<double> v1(kVectorSize);
  std::vector<double> v2(kVectorSize);

  while (state.KeepRunning())
  {
    doNaiveDotProduct(v1, v2);
  }
}
BENCHMARK(naiveDotProduct);

static void boostDotProduct(benchmark::State& state)
{
  boost::numeric::ublas::vector<double> v1(kVectorSize);
  boost::numeric::ublas::vector<double> v2(kVectorSize);

  while (state.KeepRunning())
  {
    doBoostDotProduct(v1, v2);
  }
}
BENCHMARK(boostDotProduct);

static void simdDotProduct(benchmark::State& state)
{
  std::vector<double> v1(kVectorSize);
  std::vector<double> v2(kVectorSize);

  while (state.KeepRunning())
  {
    doSimdDotProduct(v1, v2);
  }
}
BENCHMARK(simdDotProduct);

BENCHMARK_MAIN();
