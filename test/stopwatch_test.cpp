// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>
#include <fplus/stopwatch.hpp>

using namespace std::chrono_literals;

template<typename Function>
auto invoke_n_times(int n, Function f)
{
  for (int i = 0; i < n; i++) {
    f();
  }
}

TEST_CASE("Timer - test_accuracy")
{
#ifdef NDEBUG  // only on release builds
  using namespace fplus;
  using namespace std::chrono_literals;

  auto measure_delta = []() {
    fplus::stopwatch t;
    std::this_thread::sleep_for(0.05s);
    auto duration = t.elapsed();
    auto delta = duration - 0.05;
    return fabs(delta);
  };

  // we ask for a sleep of 0.05s, but we will have a duration that can be higher
  // (up to 15 ms higher, since the cpu scheduler might switch to another process during this sleep)
  // to account for the cpu/task scheduler
  double max_acceptable_delta__task_scheduler = 0.2;

  // One run
  {
    auto delta = measure_delta();
    REQUIRE_LT(delta, max_acceptable_delta__task_scheduler);
  }

  // 10 consecutive runs (total duration = 0.5 seconds)
  {
    std::vector<double> deltas;
    invoke_n_times(10, [&]() {
      deltas.push_back(measure_delta());
    });
    auto mean_dev = fplus::mean_stddev<double>(deltas);
    REQUIRE_LT(mean_dev.first, 0.09);
    REQUIRE_LT(mean_dev.second, 0.06);
  }
#endif
}

