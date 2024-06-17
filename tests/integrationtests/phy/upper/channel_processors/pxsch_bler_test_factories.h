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

#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"

namespace srsran {

/// \brief Creates a PDSCH processor factory.
/// \param[in] executor        Asynchronous executor.
/// \param[in] max_nof_threads Maximum number of concurrent threads.
/// \return A PDSCH processor factory if it is successful.
std::shared_ptr<pdsch_processor_factory> create_sw_pdsch_processor_factory(task_executor& executor,
                                                                           unsigned       max_nof_threads);

/// \brief Creates a PUSCH processor factory.
/// \param[in] executor               Asynchornous executor.
/// \param[in] max_nof_threads        Maximum number of concurrent threads.
/// \param[in] nof_ldpc_iterations    Maximum number of LDPC decoder iterations.
/// \param[in] dec_enable_early_stop  Set to true to enable LDPC decoder early stop.
/// \return A PUSCH processor factory if it is successful.
std::shared_ptr<pusch_processor_factory> create_sw_pusch_processor_factory(task_executor& executor,
                                                                           unsigned       max_nof_threads,
                                                                           unsigned       nof_ldpc_iterations,
                                                                           bool           dec_enable_early_stop);

} // namespace srsran