
#ifndef SRSGNB_LIB_RADIO_UHD_RADIO_UHD_EXCEPTION_HANDLER_H
#define SRSGNB_LIB_RADIO_UHD_RADIO_UHD_EXCEPTION_HANDLER_H

#include "radio_uhd_logger.h"
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
  bool is_on_error() const { return !error_message.empty(); }
  void clear_error() { error_message.clear(); }

  template <typename F>
  bool safe_execution(F task)
  {
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

    // No exception caught, then it clears error message.
    error_message.clear();
    return true;
  }

public:
  bool               is_successful() const { return error_message.empty(); }
  const std::string& get_error_message() { return error_message; }
};

} // namespace srsgnb
#endif // SRSGNB_LIB_RADIO_UHD_RADIO_UHD_EXCEPTION_HANDLER_H
