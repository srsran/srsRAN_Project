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

#include "srsran/du/du.h"
#include "srsran/ru/ru_adapters.h"
#include <memory>
#include <vector>

namespace srsran {

class radio_unit;

// :TODO: temportal class, as it is intended that dynamic_du_impl inherits from DU with only start and stop methods.
class flexible_du
{
public:
  virtual ~flexible_du() = default;

  /// Starts the flexible DU.
  virtual void start() = 0;

  /// Stops the flexible DU
  virtual void stop() = 0;
};

class dynamic_du_impl : public flexible_du
{
public:
  explicit dynamic_du_impl(unsigned nof_cells);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  /// Adds the given RU to this dynamic DU.
  void add_ru(std::unique_ptr<radio_unit> active_ru);

  /// Adds the given DUs to this dynamic DU.
  void add_dus(std::vector<std::unique_ptr<du>> active_du);

  /// Getters to the adaptors.
  upper_ru_ul_adapter&         get_upper_ru_ul_adapter() { return ru_ul_adapt; }
  upper_ru_timing_adapter&     get_upper_ru_timing_adapter() { return ru_timing_adapt; }
  upper_ru_error_adapter&      get_upper_ru_error_adapter() { return ru_error_adapt; }
  upper_ru_dl_rg_adapter&      get_upper_ru_dl_rg_adapter() { return ru_dl_rg_adapt; }
  upper_ru_ul_request_adapter& get_upper_ru_ul_request_adapter() { return ru_ul_request_adapt; }

private:
  upper_ru_ul_adapter              ru_ul_adapt;
  upper_ru_timing_adapter          ru_timing_adapt;
  upper_ru_error_adapter           ru_error_adapt;
  std::unique_ptr<radio_unit>      ru;
  upper_ru_dl_rg_adapter           ru_dl_rg_adapt;
  upper_ru_ul_request_adapter      ru_ul_request_adapt;
  std::vector<std::unique_ptr<du>> du_list;
};

} // namespace srsran
