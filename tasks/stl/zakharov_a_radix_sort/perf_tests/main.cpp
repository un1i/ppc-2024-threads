// Copyright 2024 Zakharov Artem
#include <gtest/gtest.h>

#include <random>

#include "core/perf/include/perf.hpp"
#include "stl/zakharov_a_radix_sort/include/radix_sort_stl.hpp"

std::vector<int> zakharov_random_vector(std::size_t sz) {
  std::mt19937 gen(2859);
  std::uniform_int_distribution<int> distrib(-100000, 100000);
  std::vector<int> result(sz);
  for (int &el : result) {
    el = distrib(gen);
  }
  return result;
}

TEST(Zakharov_a_radix_sort_stl, test_pipeline_run) {
  const int count = 40000000;

  // Create data
  std::vector<int> in(zakharov_random_vector(count));
  std::vector<int> out(count, 0);

  std::vector<int> res(in);
  sort(res.begin(), res.end());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSTL = std::make_shared<ppc::core::TaskData>();
  taskDataSTL->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataSTL->inputs_count.emplace_back(in.size());
  taskDataSTL->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataSTL->outputs_count.emplace_back(out.size());

  // Create Task
  auto zakharovTaskSTL = std::make_shared<ZakharovRadixSortSTL>(taskDataSTL);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(zakharovTaskSTL);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(out, res);
}

TEST(Zakharov_a_radix_sort_stl, test_task_run) {
  const int count = 40000000;

  // Create data
  std::vector<int> in(zakharov_random_vector(count));
  std::vector<int> out(count, 0);

  std::vector<int> res(in);
  sort(res.begin(), res.end());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSTL = std::make_shared<ppc::core::TaskData>();
  taskDataSTL->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataSTL->inputs_count.emplace_back(in.size());
  taskDataSTL->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataSTL->outputs_count.emplace_back(out.size());

  // Create Task
  auto zakharovTaskSTL = std::make_shared<ZakharovRadixSortSTL>(taskDataSTL);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(zakharovTaskSTL);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(out, res);
}
