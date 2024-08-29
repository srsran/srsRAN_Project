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
/// \param[in] executor               Asynchornous executor.
/// \param[in] max_nof_threads        Maximum number of concurrent threads.
/// \param[in] nof_ldpc_iterations    Maximum number of LDPC decoder iterations.
/// \param[in] dec_enable_early_stop  Set to true to enable LDPC decoder early stop.
/// \param[in] pxsch_type             Type of PxSCH implementation to be tested.
/// \return A PUSCH processor factory if it is successful.
std::shared_ptr<pusch_processor_factory> create_sw_pusch_processor_factory(task_executor&     executor,
                                                                           unsigned           max_nof_threads,
                                                                           unsigned           nof_ldpc_iterations,
                                                                           bool               dec_enable_early_stop,
                                                                           const std::string& pxsch_type);

} // namespace srsran
