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

#include "fmt/format.h"
#include <string>

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__
#include <uhd/error.h>
#include <uhd/exception.hpp>
#pragma GCC diagnostic pop

namespace srsran {

class uhd_exception_handler
{
public:
  template <typename S, typename... Args>
  void on_error(const S& format_str, Args&&... args)
  {
    fmt::memory_buffer str_buf;
    fmt::format_to(str_buf, format_str, std::forward<Args>(args)...);
    error_message = to_string(str_buf);
  }

  template <typename F>
  bool safe_execution(F task)
  {
    // Clears the error message.
    error_message.clear();

    static_assert(std::is_convertible<F, std::function<void()>>::value, "The function signature must be void()");
    // Try to execute task and catch exception.
    try {
      task();
    } catch (const uhd::exception& e) {
      error_message = e.what();
      return false;
    } catch (const boost::exception& e) {
      error_message = boost::diagnostic_information(e);
      return false;
    } catch (const std::exception& e) {
      error_message = e.what();
      return false;
    } catch (...) {
      error_message = "Unrecognized exception caught.";
      return false;
    }

    return is_successful();
  }

  bool               is_successful() const { return error_message.empty(); }
  const std::string& get_error_message() const { return error_message; }

private:
  std::string error_message;
};

} // namespace srsran
