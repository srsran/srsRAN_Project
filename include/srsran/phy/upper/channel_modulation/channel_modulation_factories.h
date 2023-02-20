/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include <memory>

namespace srsran {

class channel_modulation_factory
{
public:
  virtual ~channel_modulation_factory()                                     = default;
  virtual std::unique_ptr<modulation_mapper>   create_modulation_mapper()   = 0;
  virtual std::unique_ptr<demodulation_mapper> create_demodulation_mapper() = 0;
  virtual std::unique_ptr<evm_calculator>      create_evm_calculator()      = 0;
};

std::shared_ptr<channel_modulation_factory> create_channel_modulation_sw_factory();

} // namespace srsran
