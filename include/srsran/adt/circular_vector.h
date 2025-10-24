/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include <vector>

namespace srsran {

/// \brief Circular vector container.
/// \tparam T Type of elements.
/// \tparam VectorContainer Underlying vector implementation. Must provide the same interface as std::vector.
template <typename T, typename VectorContainer = std::vector<T>>
class circular_vector
{
public:
  using value_type     = T;
  using iterator       = typename VectorContainer::iterator;
  using const_iterator = typename VectorContainer::const_iterator;

  circular_vector() = default;
  circular_vector(size_t n) : data(n) {}
  circular_vector(size_t n, const T& value) : data(n, value) {}

  auto begin() { return data.begin(); }
  auto begin() const { return data.begin(); }
  auto end() { return data.end(); }
  auto end() const { return data.end(); }

  size_t size() const { return data.size(); }
  bool   empty() const { return data.empty(); }

  T&       operator[](size_t pos) { return data[pos % size()]; }
  const T& operator[](size_t pos) const { return data[pos % size()]; }

  void resize(size_t n) { data.resize(n); }
  void resize(size_t n, const T& value) { data.resize(n, value); }

  void reserve(size_t n) { data.reserve(n); }
  void push_back(const T& value) { data.push_back(value); }
  void push_back(T&& value) { data.push_back(std::move(value)); }
  template <typename... Args>
  void emplace_back(Args&&... args)
  {
    data.emplace_back(std::forward<Args>(args)...);
  }

  void clear() { data.clear(); }

private:
  VectorContainer data;
};

template <typename T, size_t N>
using static_circular_vector = circular_vector<T, static_vector<T, N>>;

} // namespace srsran
