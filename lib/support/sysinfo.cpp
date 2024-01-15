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

#include "srsran/support/sysinfo.h"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

/// Executes system command, deletes the given path if the command fails.
static bool exec_system_command(const std::string& command, const std::string& cleanup_path = "")
{
  if (::system(command.c_str()) < 0) {
    fmt::print("{} command failed. error=\"{}\"\n", command, strerror(errno));
    if (!cleanup_path.empty()) {
      ::rmdir(cleanup_path.c_str());
    }

    return false;
  }
  return true;
}

/// Writing the value 0 to a cgroup.procs file causes the writing process to be moved to the corresponding cgroup.
static void move_to_cgroup(const std::string& cgroup_path)
{
  std::ofstream output(cgroup_path + "/cgroup.procs");
  if (output.fail()) {
    fmt::print("Could not open {} for writing. error=\"{}\"\n", cgroup_path + "/cgroup.procs", strerror(errno));
  }
  output.write("0\n", 2);
}

/// Moves processes from source cgroup to destination cgroup.
static bool move_procs_between_cgroups(const std::string& dst_path, const std::string& src_path)
{
  using namespace std::chrono_literals;

  std::ifstream source_file(src_path);
  if (source_file.fail()) {
    fmt::print("Could not open {} directory. error=\"{}\"\n", src_path, strerror(errno));
    return false;
  }
  std::string pid;
  while (std::getline(source_file, pid)) {
    std::ofstream destination_file(dst_path);
    if (destination_file.fail()) {
      fmt::print("Could not open {} directory. error=\"{}\"\n", dst_path, strerror(errno));
      return false;
    }
    destination_file << std::stoi(pid) << "\n";
  }
  std::this_thread::sleep_for(50ms);

  return true;
}

bool configure_cgroups(const std::string& isol_cpus, const std::string& os_cpus)
{
  std::string cgroup_path = "/sys/fs/cgroup";
  struct stat info;
  if (::stat(cgroup_path.c_str(), &info) < 0) {
    fmt::print("Could not find {}, make sure cgroups-v2 is mounted. error=\"{}\"\n", cgroup_path, strerror(errno));
    return false;
  }

  /// First move itself to root cgroup.
  move_to_cgroup(cgroup_path);

  /// Create cgroup for OS tasks, call it 'housekeeping' cgroup.
  if (!os_cpus.empty()) {
    std::string housekeeping = cgroup_path + "/housekeeping";
    if ((::mkdir(housekeeping.c_str(), 0755)) < 0 && errno != EEXIST) {
      fmt::print("Could not create {} directory. error=\"{}\"\n", housekeeping, strerror(errno));
      return false;
    }
    std::string set_cpus_cmd = "echo " + os_cpus + " > " + housekeeping + "/cpuset.cpus";
    if (!exec_system_command(set_cpus_cmd, housekeeping)) {
      return false;
    }
    /// Migrate all processes to the default cgroup, that will be using housekeeping cpuset.
    std::string procs_filename = housekeeping + "/cgroup.procs";
    FILE*       file           = popen("ps -eLo lwp=", "r");
    if (!file) {
      fmt::print("Couldn't move system processes to a dedicated cgroup\n");
      return false;
    }
    const size_t len = 32;
    char         pid_buffer[len];
    while (::fgets(pid_buffer, len, file)) {
      unsigned          pid;
      std::stringstream ss(pid_buffer);
      std::ofstream     output(procs_filename);
      if (output.fail()) {
        fmt::print("Could not open {} directory. error=\"{}\"\n", procs_filename, strerror(errno));
        return false;
      }
      ss >> pid;
      output << pid << "\n";
    }
    ::pclose(file);
  }

  /// Create cgroup with isolated CPUs dedicated for the gNB app.
  std::string isol_cgroup_path = cgroup_path + "/isolated";
  if ((::mkdir(isol_cgroup_path.c_str(), 0755)) < 0 && errno != EEXIST) {
    fmt::print("Could not create {} directory. error=\"{}\"\n", isol_cgroup_path, strerror(errno));
    return false;
  }

  std::string set_cpus_cmd = "echo " + isol_cpus + " > " + isol_cgroup_path + "/cpuset.cpus";
  if (!exec_system_command(set_cpus_cmd, isol_cgroup_path)) {
    return false;
  }

  /// Finally move itself to isolcated cgroup.
  move_to_cgroup(isol_cgroup_path);

  return true;
}

void cleanup_cgroups()
{
  using namespace std::chrono_literals;

  bool        cgroup_changed           = false;
  std::string isolated_cgroup_path     = "/sys/fs/cgroup/isolated";
  std::string housekeeping_cgroup_path = "/sys/fs/cgroup/housekeeping";
  std::string root_cgroup_path         = "/sys/fs/cgroup/cgroup.procs";

  struct stat sysfs_info;
  if (::stat("/sys/fs/cgroup", &sysfs_info) < 0) {
    return;
  }

  struct stat info;
  if (::stat(housekeeping_cgroup_path.c_str(), &info) == 0) {
    move_procs_between_cgroups(root_cgroup_path, housekeeping_cgroup_path + "/cgroup.procs");
    // Remove previously created cgroups.
    if (::rmdir(housekeeping_cgroup_path.c_str()) < 0) {
      fmt::print("Could not delete {}. error=\"{}\"\n", housekeeping_cgroup_path, strerror(errno));
    }
    cgroup_changed = true;
  }
  if (::stat(isolated_cgroup_path.c_str(), &info) == 0) {
    // Move all processes to the parent cgroup.
    move_procs_between_cgroups(root_cgroup_path, isolated_cgroup_path + "/cgroup.procs");
    if (::rmdir(isolated_cgroup_path.c_str())) {
      fmt::print("Could not delete {}. error=\"{}\"\n", isolated_cgroup_path, strerror(errno));
    }
    cgroup_changed = true;
  }
  // Wait for changes in cpuset to take place.
  if (cgroup_changed) {
    std::this_thread::sleep_for(100ms);
  }
  move_to_cgroup("/sys/fs/cgroup");
}
