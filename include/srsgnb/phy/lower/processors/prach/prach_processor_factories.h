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

#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/phy/lower/prach_processor.h"
#include <memory>

namespace srsgnb {

class task_executor;

/// PRACH processor factory.
class prach_processor_factory
{
public:
  /// Default destructor.
  virtual ~prach_processor_factory() = default;

  /// Creates a lower PHY PRACH processor.
  virtual std::unique_ptr<prach_processor> create(task_executor& async_task_executor) = 0;
};

/// Creates a software based PRACH processor.
std::shared_ptr<prach_processor_factory>
create_prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory,
                                  unsigned                                        dft_size_15kHz,
                                  unsigned                                        max_nof_concurrent_requests);

} // namespace srsgnb