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

#include "srsran/support/compiler.h"
#include "fmt/format.h"
#include <unistd.h>

namespace srsran {

/// \brief Scoped file descriptor that calls ::close(fd) on destruction and is not copyable.
class unique_fd
{
public:
  unique_fd() = default;
  explicit unique_fd(int fd_) : fd(fd_) {}
  unique_fd(const unique_fd& other) = delete;
  unique_fd(unique_fd&& other) noexcept : fd(other.fd) { other.fd = -1; }
  unique_fd& operator=(const unique_fd& other) = delete;
  unique_fd& operator=(unique_fd&& other) noexcept
  {
    if (is_open()) {
      if (not close()) {
        fmt::print("Error: Failed to close file descriptor: {}.", strerror(errno));
      }
    }
    fd       = other.fd;
    other.fd = -1;
    return *this;
  }
  ~unique_fd()
  {
    bool ret = close();
    if (not ret) {
      fmt::print("Error: Failed to close file descriptor: {}.", strerror(errno));
    }
  }

  SRSRAN_NODISCARD bool close()
  {
    if (fd >= 0) {
      if (::close(fd) == -1) {
        fd = -1;
        return false;
      }
      fd = -1;
    }
    return true;
  }

  SRSRAN_NODISCARD bool is_open() const { return fd >= 0; }

  SRSRAN_NODISCARD int value() const { return fd; }

private:
  int fd = -1;
};

} // namespace srsran