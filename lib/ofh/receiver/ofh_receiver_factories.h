/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../support/prach_context_repository.h"
#include "../support/uplink_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "../support/uplink_notified_grid_symbol_repository.h"
#include "ofh_receiver_impl.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"
#include <memory>

namespace srsran {

class task_executor;

namespace ofh {

/// Creates a receiver with the given configuration and dependencies.
std::unique_ptr<receiver> create_receiver(const receiver_config&                                  receiver_cfg,
                                          srslog::basic_logger&                                   logger,
                                          task_executor&                                          uplink_executor,
                                          std::unique_ptr<ether::receiver>                        eth_receiver,
                                          uplane_rx_symbol_notifier*                              notifier,
                                          std::shared_ptr<prach_context_repository>               prach_context_repo,
                                          std::shared_ptr<uplink_context_repository>              ul_slot_context_repo,
                                          std::shared_ptr<uplink_cplane_context_repository>       ul_cp_context_repo,
                                          std::shared_ptr<uplink_cplane_context_repository>       prach_cp_context_repo,
                                          std::shared_ptr<uplink_notified_grid_symbol_repository> notifier_symbol_repo);

} // namespace ofh
} // namespace srsran
