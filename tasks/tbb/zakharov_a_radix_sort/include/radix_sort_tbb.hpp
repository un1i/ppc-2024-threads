// Copyright 2024 Zakharov Artem
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

class ZakharovRadixSortTBB : public ppc::core::Task {
  union Number {
    Number(){};
    int32_t number{};
    const uint8_t bytes[sizeof(int32_t)];
  };

 public:
  explicit ZakharovRadixSortTBB(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Number> inp_arr;
  Number* out_arr{};
  std::size_t arr_size{};
  std::size_t portion{0};
  bool res_in_out{true};
  static const std::size_t counter_size = 256;
  static const std::size_t num_bytes = sizeof(int32_t);
  std::pair<Number*, Number*> radix_sort_parallel(Number* src, Number* tmp, std::size_t size);
  static void radix_sort_seq(Number* src, Number* dest, std::size_t size);
  static void counting_sort_by_bytes(const Number* src, Number* dest, std::size_t byte_num, std::size_t size,
                                     std::vector<size_t>& counter);
  static void merge(const Number* src1, std::size_t size1, const Number* src2, std::size_t size2, Number* dest);
  static size_t get_index(const Number* arr, std::size_t arr_ind, std::size_t byte_num);
  static void copy_data(const Number* src, Number* dist, std::size_t size);
};
