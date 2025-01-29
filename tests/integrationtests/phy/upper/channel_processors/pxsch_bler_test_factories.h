/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"

namespace srsran {

/// \brief Creates a PDSCH processor factory.
/// \param[in] executor        Asynchronous executor.
/// \param[in] max_nof_threads Maximum number of concurrent threads.
/// \param[in] eal_arguments   String containing the EAL arguments to be used in case of hardware-acceleration.
/// \param[in] pxsch_type      Type of PxSCH implementation to be tested.
/// \return A PDSCH processor factory if it is successful.
std::shared_ptr<pdsch_processor_factory> create_sw_pdsch_processor_factory(task_executor&     executor,
                                                                           unsigned           max_nof_threads,
                                                                           const std::string& eal_arguments,
                                                                           const std::string& pxsch_type);

/// \brief Creates a PUSCH processor factory.
/// \param[in] executor                  Asynchornous executor.
/// \param[in] max_nof_threads           Maximum number of concurrent threads.
/// \param[in] nof_ldpc_iterations       Maximum number of LDPC decoder iterations.
/// \param[in] pxsch_type                Type of PxSCH implementation to be tested.
/// \param[in] dec_enable_early_stop     Set to true to enable LDPC decoder early stop.
/// \param[in] td_interpolation_strategy Channel estimator time-domain interpolation strategy.
/// \return A PUSCH processor factory if it is successful.
std::shared_ptr<pusch_processor_factory>
create_sw_pusch_processor_factory(task_executor&                                   executor,
                                  unsigned                                         max_nof_threads,
                                  unsigned                                         nof_ldpc_iterations,
                                  bool                                             dec_enable_early_stop,
                                  const std::string&                               pxsch_type,
                                  port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
                                  channel_equalizer_algorithm_type                 equalizer_algorithm_type);

} // namespace srsran
