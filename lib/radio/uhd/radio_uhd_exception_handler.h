/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_uhd_logger.h"
#include "fmt/format.h"
#include <boost/exception/diagnostic_information.hpp>
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

namespace srsgnb {

class uhd_exception_handler
{
private:
  std::string error_message;

protected:
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

public:
  bool               is_successful() const { return error_message.empty(); }
  const std::string& get_error_message() const { return error_message; }
};

} // namespace srsgnb
