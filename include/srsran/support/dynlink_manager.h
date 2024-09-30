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
