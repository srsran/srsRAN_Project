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

#pragma once

#include "srsran/adt/bounded_bitset.h"

namespace srsran {

/// Class discovering the CPU architecture of the underlying hardware.
class cpu_architecture_info
{
  /// Aggregates CPU information.
  struct cpu_description {
    /// Set of CPUs as returned by the OS.
    cpu_set_t cpuset;
    /// Total number of CPUs.
    size_t nof_cpus;
    /// Number of available CPUs.
    size_t nof_available_cpus;
    /// Highest index of the available CPU.
    size_t max_cpu_id;
    /// List of CPUs that the application is allowed to use.
    bounded_bitset<1024> allowed_cpus;
    /// Number of NUMA nodes.
    size_t nof_numa_nodes;
    /// List of CPUs pertaining to each NUMA node.
    std::vector<bounded_bitset<1024>> node_cpus;
    /// Lists of logical CPUs belonging to each physical CPU.
    std::vector<bounded_bitset<1024>> logical_cpu_lists;
  };

  /// Discovers CPU architecture at the application start-up.
  static cpu_description discover_cpu_architecture();

  /// Stores the CPU description.
  static const cpu_description cpu_desc;

  /// Default constructor.
  cpu_architecture_info() = default;

public:
  cpu_architecture_info(const cpu_architecture_info& other) = delete;
  cpu_architecture_info(cpu_architecture_info&& other)      = delete;
  void operator=(const cpu_architecture_info& other)        = delete;
  void operator=(cpu_architecture_info&& other)             = delete;

  /// Returns reference to a static instance of this class.
  static cpu_architecture_info& get()
  {
    /// Singleton object.
    static cpu_architecture_info cpu_info;
    return cpu_info;
  }

  /// Get total number of CPUs discovered in the given host.
  size_t get_host_total_nof_cpus() const { return cpu_desc.nof_cpus; }

  /// Get the number of CPUs available to the application.
  size_t get_host_nof_available_cpus() const { return cpu_desc.nof_available_cpus; }

  /// Get available CPUs as a cpu_set_t structure.
  cpu_set_t get_available_cpuset() const { return cpu_desc.cpuset; }

  /// Prints discovered CPU information.
  void print_cpu_info(srslog::basic_logger& logger) const;

  /// Returns CPUs of the given NUMA node.
  bounded_bitset<1024> get_node_cpu_mask(unsigned node_id) const;

  /// Run calling thread and its children and allocate memory on the given NUMA node.
  bool run_on_numa_node(unsigned node_id) const;
};

} // namespace srsran
