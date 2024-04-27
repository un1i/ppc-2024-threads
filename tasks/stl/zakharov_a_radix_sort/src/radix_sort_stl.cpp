// Copyright 2024 Zakharov Artem
#include "stl/zakharov_a_radix_sort/include/radix_sort_stl.hpp"

#include <cmath>
#include <future>

bool ZakharovRadixSortSTL::validation() {
  internal_order_test();
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool ZakharovRadixSortSTL::pre_processing() {
  internal_order_test();
  try {
    arr_size = taskData->inputs_count[0];
    auto* inp = reinterpret_cast<Number*>(taskData->inputs[0]);
    inp_arr.resize(arr_size);
    copy_data(inp, inp_arr.data(), arr_size);
    out_arr = reinterpret_cast<Number*>(taskData->outputs[0]);
    int max_threads = static_cast<int>(std::pow(2, static_cast<int>(std::log2(std::thread::hardware_concurrency()))));
    portion = arr_size / max_threads;
    if (arr_size % max_threads != 0) {
      portion++;
    }

    return true;
  } catch (...) {
    return false;
  }
}

bool ZakharovRadixSortSTL::run() {
  internal_order_test();
  try {
    auto res = radix_sort_parallel(inp_arr.data(), out_arr, arr_size);
    if (res.first != out_arr) {
      res_in_out = false;
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool ZakharovRadixSortSTL::post_processing() {
  internal_order_test();
  try {
    if (!res_in_out) {
      copy_data(inp_arr.data(), out_arr, arr_size);
    }
    return true;
  } catch (...) {
    return false;
  }
}

size_t ZakharovRadixSortSTL::get_index(const Number* const arr, std::size_t arr_ind, std::size_t byte_num) {
  if (byte_num == num_bytes - 1) {
    return static_cast<int8_t>(arr[arr_ind].bytes[byte_num]) + 128;
  }
  return arr[arr_ind].bytes[byte_num];
}

std::pair<ZakharovRadixSortSTL::Number*, ZakharovRadixSortSTL::Number*> ZakharovRadixSortSTL::radix_sort_parallel(
    Number* src, Number* tmp, std::size_t size) {
  if (size <= portion) {
    radix_sort_seq(src, tmp, size);
    return {src, tmp};
  }

  size_t size1 = size / 2;
  size_t size2 = size - size1;
  std::pair<Number*, Number*> data1;
  std::pair<Number*, Number*> data2;

  auto future = std::async(std::launch::async, [&] { return radix_sort_parallel(src, tmp, size1); });
  data2 = radix_sort_parallel(src + size1, tmp + size1, size2);
  data1 = future.get();

  merge(data1.first, size1, data2.first, size2, data1.second);
  return {data1.second, data1.first};
}

void ZakharovRadixSortSTL::radix_sort_seq(Number* src, Number* dest, std::size_t size) {
  std::vector<size_t> counter(counter_size);
  for (size_t i = 0; i < num_bytes; i++) {
    counting_sort_by_bytes(src, dest, i, size, counter);
    std::swap(src, dest);
  }
}

void ZakharovRadixSortSTL::merge(const Number* src1, std::size_t size1, const Number* src2, std::size_t size2,
                                 Number* dest) {
  size_t i = 0;
  size_t j = 0;
  size_t dest_ind = 0;
  while (dest_ind < size1 + size2) {
    if (j == size2 || (i != size1 && src1[i].number <= src2[j].number)) {
      dest[dest_ind++].number = src1[i++].number;
    } else {
      dest[dest_ind++].number = src2[j++].number;
    }
  }
}

void ZakharovRadixSortSTL::counting_sort_by_bytes(const Number* const src, Number* const dest, std::size_t byte_num,
                                                  std::size_t size, std::vector<size_t>& counter) {
  std::fill(counter.begin(), counter.end(), 0);

  // count numbers
  for (std::size_t i = 0; i < size; i++) {
    std::size_t ind = get_index(src, i, byte_num);
    counter[ind]++;
  }

  // generate offsets
  std::size_t prev = 0;
  for (auto& el : counter) {
    std::swap(prev, el);
    prev += el;
  }

  // sort elements
  for (std::size_t i = 0; i < size; i++) {
    std::size_t ind = get_index(src, i, byte_num);
    dest[counter[ind]++].number = src[i].number;
  }
}

void ZakharovRadixSortSTL::copy_data(const Number* const src, Number* const dist, std::size_t size) {
  for (std::size_t i = 0; i < size; i++) {
    dist[i].number = src[i].number;
  }
}
