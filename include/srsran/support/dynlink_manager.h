/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/srslog/logger.h"

namespace srsran {
class dynlink_manager
{
public:
  static std::optional<dynlink_manager> create(const std::string& dl_name, srslog::basic_logger& logger_);
  expected<void*>                       load_symbol(const std::string& symbol_name);

  ~dynlink_manager();

private:
  explicit dynlink_manager(const std::string& dl_name_, srslog::basic_logger& logger_);
  [[nodiscard]] bool open();
  bool               close();

  void*                 dl_handle = nullptr;
  std::string           dl_name;
  srslog::basic_logger& logger;
};

} // namespace srsran
