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

#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor.h"
#include <memory>

namespace srsran {

class task_executor;
class sampling_rate;

/// Lower physical layer PRACH processor - Factory interface.
class prach_processor_factory
{
public:
  /// Default destructor.
  virtual ~prach_processor_factory() = default;

  /// Creates a lower PHY PRACH processor.
  virtual std::unique_ptr<prach_processor> create() = 0;
};

/// \brief Creates a software based PRACH processor factory.
///
/// \param ofdm_prach_factory          OFDM demodulator for PRACH factory.
/// \param async_task_executor         Task executor for demodulating PRACH asynchronously.
/// \param srate                       Sampling rate.
/// \param max_nof_ports               Maximum number of PRACH reception ports.
/// \param max_nof_concurrent_requests Maximum number of concurrent PRACH processing requests.
/// \return A valid PRACH processor factory.
std::shared_ptr<prach_processor_factory>
create_prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory,
                                  task_executor&                                  async_task_executor,
                                  sampling_rate                                   srate,
                                  unsigned                                        max_nof_ports,
                                  unsigned                                        max_nof_concurrent_requests);

} // namespace srsran