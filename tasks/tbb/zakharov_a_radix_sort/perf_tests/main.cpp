// Copyright 2024 Zakharov Artem
#include <gtest/gtest.h>
#include <oneapi/tbb.h>
#include <random>

#include "core/perf/include/perf.hpp"
#include "tbb/zakharov_a_radix_sort/include/radix_sort_tbb.hpp"

std::vector<int> zakharov_random_vector(std::size_t sz) {
  std::mt19937 gen(2859);
  std::uniform_int_distribution<int> distrib(-100000, 100000);
  std::vector<int> result(sz);
  for (int &el : result) {
    el = distrib(gen);
  }
  return result;
}

TEST(Zakharov_a_radix_sort_tbb, test_pipeline_run) {
  const int count = 40000000;

  // Create data
  std::vector<int> in(zakharov_random_vector(count));
  std::vector<int> out(count, 0);

  std::vector<int> res(in);
  sort(res.begin(), res.end());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataTBB = std::make_shared<ppc::core::TaskData>();
  taskDataTBB->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataTBB->inputs_count.emplace_back(in.size());
  taskDataTBB->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataTBB->outputs_count.emplace_back(out.size());

  // Create Task
  auto zakharovTaskTBB = std::make_shared<ZakharovRadixSortTBB>(taskDataTBB);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = oneapi::tbb::tick_count::now();
  perfAttr->current_timer = [&] { return (oneapi::tbb::tick_count::now() - t0).seconds(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(zakharovTaskTBB);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(out, res);
}

TEST(Zakharov_a_radix_sort_tbb, test_task_run) {
  const int count = 40000000;

  // Create data
  std::vector<int> in(zakharov_random_vector(count));
  std::vector<int> out(count, 0);

  std::vector<int> res(in);
  sort(res.begin(), res.end());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataTBB = std::make_shared<ppc::core::TaskData>();
  taskDataTBB->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataTBB->inputs_count.emplace_back(in.size());
  taskDataTBB->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataTBB->outputs_count.emplace_back(out.size());

  // Create Task
  auto zakharovTaskTBB = std::make_shared<ZakharovRadixSortTBB>(taskDataTBB);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = oneapi::tbb::tick_count::now();
  perfAttr->current_timer = [&] { return (oneapi::tbb::tick_count::now() - t0).seconds(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(zakharovTaskTBB);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(out, res);
}
