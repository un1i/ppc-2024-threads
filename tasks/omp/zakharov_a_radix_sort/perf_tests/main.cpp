// Copyright 2024 Zakharov Artem
#include <gtest/gtest.h>
#include <omp.h>

#include <random>

#include "core/perf/include/perf.hpp"
#include "omp/zakharov_a_radix_sort/include/radix_sort_omp.hpp"

std::vector<int> zakharov_random_vector(std::size_t sz) {
  std::mt19937 gen(2859);
  std::uniform_int_distribution<int> distrib(-100000, 100000);
  std::vector<int> result(sz);
  for (int &el : result) {
    el = distrib(gen);
  }
  return result;
}

TEST(Zakharov_a_radix_sort_omp, test_pipeline_run) {
  const int count = 40000000;

  // Create data
  std::vector<int> in(zakharov_random_vector(count));
  std::vector<int> out(count, 0);

  std::vector<int> res(in);
  sort(res.begin(), res.end());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataOMP = std::make_shared<ppc::core::TaskData>();
  taskDataOMP->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataOMP->inputs_count.emplace_back(in.size());
  taskDataOMP->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataOMP->outputs_count.emplace_back(out.size());

  // Create Task
  auto zakharovTaskOMP = std::make_shared<ZakharovRadixSortOMP>(taskDataOMP);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  perfAttr->current_timer = [&] { return omp_get_wtime(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(zakharovTaskOMP);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(out, res);
}

TEST(Zakharov_a_radix_sort_omp, test_task_run) {
  const int count = 40000000;

  // Create data
  std::vector<int> in(zakharov_random_vector(count));
  std::vector<int> out(count, 0);

  std::vector<int> res(in);
  sort(res.begin(), res.end());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataOMP = std::make_shared<ppc::core::TaskData>();
  taskDataOMP->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataOMP->inputs_count.emplace_back(in.size());
  taskDataOMP->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataOMP->outputs_count.emplace_back(out.size());

  // Create Task
  auto zakharovTaskOMP = std::make_shared<ZakharovRadixSortOMP>(taskDataOMP);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  perfAttr->current_timer = [&] { return omp_get_wtime(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(zakharovTaskOMP);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(out, res);
}
