/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cli11_cpu_affinities_parser_helper.h"
#include "apps/services/os_sched_affinity_manager.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/interval.h"

using namespace srsran;

template <typename Integer>
static expected<Integer, std::string> parse_int(const std::string& value)
{
  try {
    return std::stoi(value);
  } catch (const std::invalid_argument& e) {
    return make_unexpected(e.what());
  } catch (const std::out_of_range& e) {
    return make_unexpected(e.what());
  }
}

static error_type<std::string> is_valid_cpu_index(unsigned cpu_idx)
{
  std::string error_message = fmt::format("Invalid CPU core selected '{}'. Valid CPU ids: {}",
                                          cpu_idx,
                                          os_sched_affinity_bitmask::available_cpus().get_cpu_ids());

  os_sched_affinity_bitmask one_cpu_mask;
  if (cpu_idx >= one_cpu_mask.size()) {
    return make_unexpected(error_message);
  }
  one_cpu_mask.set(cpu_idx);
  if (not one_cpu_mask.subtract(os_sched_affinity_bitmask::available_cpus()).empty()) {
    return make_unexpected(error_message);
  }
  return default_success_t();
}

static expected<unsigned, std::string> parse_one_cpu(const std::string& value)
{
  expected<int, std::string> result = parse_int<int>(value);

  if (not result.has_value()) {
    return make_unexpected(fmt::format("Could not parse '{}' string as a CPU index", value));
  }

  error_type<std::string> validation_result = is_valid_cpu_index(result.value());
  if (not validation_result.has_value()) {
    return make_unexpected(validation_result.error());
  }

  return result.value();
}

static expected<interval<unsigned, true>, std::string> parse_cpu_range(const std::string& value)
{
  std::vector<unsigned> range;
  std::stringstream     ss(value);
  while (ss.good()) {
    std::string str;
    getline(ss, str, '-');
    auto parse_result = parse_one_cpu(str);
    if (not parse_result.has_value()) {
      return make_unexpected(fmt::format("{}. Could not parse '{}' as a range", parse_result.error(), value));
    }

    range.push_back(parse_result.value());
  }

  // A range is defined by two numbers.
  if (range.size() != 2) {
    return make_unexpected(fmt::format("Could not parse '{}' as a range", value));
  }

  if (range[1] <= range[0]) {
    return make_unexpected(fmt::format("Invalid CPU core range detected [{}-{}]", range[0], range[1]));
  }

  return interval<unsigned, true>(range[0], range[1]);
}

void srsran::parse_affinity_mask(os_sched_affinity_bitmask& mask,
                                 const std::string&         value,
                                 const std::string&         property_name)
{
  std::stringstream ss(value);

  while (ss.good()) {
    std::string str;
    getline(ss, str, ',');
    if (str.find('-') != std::string::npos) {
      auto range = parse_cpu_range(str);
      if (not range.has_value()) {
        report_error("{} in the '{}' property", range.error(), property_name);
      }

      // Add 1 to the stop value as the fill method excludes the end position.
      mask.fill(range.value().start(), range.value().stop() + 1);
    } else {
      auto cpu_idx = parse_one_cpu(str);
      if (not cpu_idx.has_value()) {
        report_error("{} in the '{}' property", cpu_idx.error(), property_name);
      }

      mask.set(cpu_idx.value());
    }
  }
}
