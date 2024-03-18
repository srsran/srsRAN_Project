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

#include "srsran/support/cpu_architecture_info.h"
#include "srsran/adt/interval.h"
#include "srsran/support/format_utils.h"
#include "srsran/support/sysinfo.h"
#include <dirent.h>
#include <fstream>
#include <regex>
#include <set>
#include <string>
#include <sys/stat.h>
#ifdef NUMA_SUPPORT
#include <numa.h>
#endif

using namespace srsran;

/// Converts the string containing a CPU index to an unsigned integer number.
static unsigned parse_one_cpu(const std::string& value)
{
  return std::stoi(value);
}

/// Parses string representing range of CPU indexes in the format 'M-N'.
static interval<unsigned, true> parse_cpu_range(const std::string& value)
{
  std::vector<unsigned> range;
  std::stringstream     ss(value);

  while (ss.good()) {
    std::string str;
    std::getline(ss, str, '-');
    auto parse_result = parse_one_cpu(str);
    range.push_back(parse_result);
  }
  return interval<unsigned, true>(range[0], range[1]);
}

// Obtain CPU description at the start of the application. This value is affected by commands or tools like taskset,
// which limit the number of cores available to the application. However, frameworks (e.g. DPDK) that affect the
// affinities of the main thread in the main() function will not affect this value.
const cpu_architecture_info::cpu_description cpu_architecture_info::cpu_desc =
    cpu_architecture_info::discover_cpu_architecture();

cpu_architecture_info::cpu_description cpu_architecture_info::discover_cpu_architecture()
{
  // Clean-up cgroups possibly left from a previous run.
  cleanup_cgroups();

  cpu_description cpuinfo;
  cpu_set_t&      cpuset = cpuinfo.cpuset;

  // Discover host CPU architecture.
  CPU_ZERO(&cpuset);
  cpuinfo.allowed_cpus.resize(CPU_SETSIZE);

  if (::sched_getaffinity(0, sizeof(cpuset), &cpuset) == 0) {
    cpuinfo.nof_available_cpus = CPU_COUNT(&cpuset);
    // Save bitmask of CPUs the calling thread is allowed to run on.
    for (unsigned i = 0; i != CPU_SETSIZE; ++i) {
      if (CPU_ISSET(i, &cpuset)) {
        cpuinfo.allowed_cpus.set(i);
      }
    }
  } else {
    // Parse /proc/self/status
    std::ifstream file("/proc/self/status");
    std::string   line;
    while (std::getline(file, line)) {
      // Look for allowed CPUs mask in hexadecimal format.
      if (line.find("Cpus_allowed_list") != std::string::npos) {
        const std::regex re("([0-9]+)(.*)$");
        std::smatch      m;
        std::regex_search(line, m, re);
        std::istringstream iss(line.substr(m.position()));
        while (iss.good()) {
          std::string str;
          std::getline(iss, str, ',');
          if (str.find('-') != std::string::npos) {
            auto range = parse_cpu_range(str);
            cpuinfo.allowed_cpus.fill(range.start(), range.stop() + 1);
          } else {
            auto cpu_idx = parse_one_cpu(str);
            cpuinfo.allowed_cpus.set(cpu_idx);
          }
        }
        break;
      }
    }
  }
  cpuinfo.nof_available_cpus = cpuinfo.allowed_cpus.count();
  cpuinfo.max_cpu_id         = cpuinfo.allowed_cpus.find_highest();
  cpuinfo.nof_cpus           = cpuinfo.max_cpu_id + 1;
  cpuinfo.allowed_cpus.resize(cpuinfo.nof_cpus);

  // Parse /sys/devices/system/cpu/cpu*/topology/thread_siblings_list to get the lists of logical cores belonging to
  // the same physical core.
  auto sort_function = [](std::string a, std::string b) {
    auto get_first_cpu = [](const std::string& line) {
      std::string        str;
      std::istringstream iss(line);
      std::getline(iss, str, ',');
      if (str.find('-') != std::string::npos) {
        std::getline(iss, str, '-');
      }
      return parse_one_cpu(str);
    };
    unsigned cpu_id_a = get_first_cpu(a);
    unsigned cpu_id_b = get_first_cpu(b);
    return cpu_id_a < cpu_id_b;
  };
  std::set<std::string, decltype(sort_function)> thread_siblings_set(sort_function);
  std::string                                    cpu_system_path = "/sys/devices/system/cpu";
  ::DIR*                                         dir             = ::opendir(cpu_system_path.c_str());
  if (dir) {
    ::dirent*     entry;
    struct ::stat info;

    while ((entry = ::readdir(dir))) {
      const std::regex re("^cpu[0-9]+");
      std::string      cpu_name = entry->d_name;
      if (!std::regex_match(cpu_name, re)) {
        continue;
      }
      std::string cpu_siblings_path = fmt::format("{}/{}/topology/thread_siblings_list", cpu_system_path, cpu_name);
      if (::stat(cpu_siblings_path.c_str(), &info) < 0) {
        continue;
      }
      std::ifstream file(cpu_siblings_path);
      std::string   str_mask;
      std::getline(file, str_mask);
      thread_siblings_set.insert(str_mask);
    }
    ::closedir(dir);
  }
  for (const auto& mask : thread_siblings_set) {
    cpuinfo.logical_cpu_lists.emplace_back();
    auto& bitmask = cpuinfo.logical_cpu_lists.back();
    bitmask.resize(CPU_SETSIZE);

    std::istringstream iss(mask);
    while (iss.good()) {
      std::string str;
      std::getline(iss, str, ',');
      if (str.find('-') != std::string::npos) {
        auto range = parse_cpu_range(str);
        bitmask.fill(range.start(), range.stop() + 1);
      } else {
        auto cpu_idx = parse_one_cpu(str);
        bitmask.set(cpu_idx);
      }
    }
  }

#ifdef NUMA_SUPPORT
  if (::numa_available() == -1) {
    return cpuinfo;
  }
  cpuinfo.nof_cpus           = ::numa_num_configured_cpus();
  cpuinfo.nof_available_cpus = ::numa_num_task_cpus();
  cpuinfo.nof_numa_nodes     = ::numa_num_configured_nodes();

  // Save bitmasks of the CPUs pertaining to each NUMA node.
  for (unsigned node = 0; node != cpuinfo.nof_numa_nodes; ++node) {
    ::bitmask* mask = ::numa_allocate_cpumask();
    ::numa_node_to_cpus(node, mask);

    cpuinfo.node_cpus.emplace_back(numa_num_configured_cpus());
    cpuinfo.node_cpus[node].from_uint64(*mask->maskp);
    ::numa_free_cpumask(mask);
  }
#else
  std::string node_system_path = "/sys/devices/system/node";
  dir                          = ::opendir(node_system_path.c_str());
  if (dir) {
    ::dirent* entry;
    while ((entry = ::readdir(dir))) {
      const std::regex re("^node[0-9]+");
      std::string      entry_name = entry->d_name;
      if (std::regex_match(entry_name, re)) {
        ++cpuinfo.nof_numa_nodes;
      }
    }
    ::closedir(dir);
  }
#endif // NUMA_SUPPORT

  return cpuinfo;
}

static std::string print_cpus_list(const bounded_bitset<1024>& cpus_mask)
{
  auto cpu_ids = cpus_mask.get_bit_positions();
  if (cpu_ids.empty()) {
    return "[]";
  }
  fmt::memory_buffer fmt_format_buf;
  fmt::format_to(fmt_format_buf, "[");
  for (unsigned idx = 0, e = cpu_ids.size(); idx != e; ++idx) {
    fmt::format_to(fmt_format_buf, "{}{}", cpu_ids[idx], (idx == cpu_ids.size() - 1) ? "]" : ",");
  }
  return to_string(fmt_format_buf);
}

void cpu_architecture_info::print_cpu_info(srslog::basic_logger& logger) const
{
  fmt::memory_buffer fmt_buf;
  fmt::format_to(fmt_buf,
                 "{} {}, {} NUMA {}.\n",
                 cpu_desc.nof_cpus,
                 (cpu_desc.nof_cpus > 1) ? "CPUs" : "CPU",
                 cpu_desc.nof_numa_nodes,
                 cpu_desc.nof_numa_nodes > 1 ? "nodes" : "node");

#ifdef NUMA_SUPPORT
  fmt::format_to(fmt_buf, "CPUs per NUMA node:\n{{");
  for (unsigned node = 0; node != cpu_desc.nof_numa_nodes; ++node) {
    fmt::format_to(fmt_buf, "\n   Node {} CPUs: {}", node, print_cpus_list(cpu_desc.node_cpus[node]));
  }
  fmt::format_to(fmt_buf, "\n}}\n");
#endif

  fmt::format_to(fmt_buf, "CPUs per each physical CPU core:\n{{");
  for (unsigned core_id = 0, e = cpu_desc.logical_cpu_lists.size(); core_id != e; ++core_id) {
    fmt::format_to(fmt_buf, "\n   {}: {}", core_id, print_cpus_list(cpu_desc.logical_cpu_lists[core_id]));
  }
  fmt::format_to(fmt_buf, "\n}}\n");
  fmt::format_to(fmt_buf,
                 "{} CPUs available to the application: {}",
                 cpu_desc.nof_available_cpus,
                 print_cpus_list(cpu_desc.allowed_cpus));

  logger.debug("Detected CPU topology: {}", to_c_str(fmt_buf));
}

bounded_bitset<1024> cpu_architecture_info::get_node_cpu_mask(unsigned node_id) const
{
#ifndef NUMA_SUPPORT
  srslog::fetch_basic_logger("GNB").debug("NUMA topology not available, please compile the code with libnuma support");
  return {};
#else

  if (node_id >= cpu_desc.nof_numa_nodes) {
    srslog::fetch_basic_logger("GNB").debug("Requested node ID exceeds number of NUMA nodes configured in the system");
    return {};
  }
  return cpu_desc.node_cpus[node_id];
#endif
}

bool cpu_architecture_info::run_on_numa_node(unsigned node_id) const
{
#ifndef NUMA_SUPPORT
  srslog::fetch_basic_logger("GNB").debug("NUMA topology not available, please compile the code with libnuma support");
  return false;
#else

  if (node_id >= cpu_desc.nof_numa_nodes) {
    srslog::fetch_basic_logger("GNB").debug("Requested node ID exceeds number of NUMA nodes configured in the system");
    return false;
  }
  ::bitmask* mask = ::numa_allocate_nodemask();
  ::numa_bitmask_setbit(mask, node_id);
  ::numa_bind(mask);
  ::numa_free_nodemask(mask);
  return true;
#endif
}
