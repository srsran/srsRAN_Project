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
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class task_executor;

namespace ether {
class eth_frame_pool;
}

namespace ofh {

/// Creates a transmitter with the given configuration and dependencies.
std::unique_ptr<transmitter>
create_transmitter(const transmitter_config&                         transmitter_cfg,
                   srslog::basic_logger&                             logger,
                   task_executor&                                    tx_executor,
                   task_executor&                                    downlink_executor,
                   std::unique_ptr<ether::gateway>                   eth_gateway,
                   std::shared_ptr<prach_context_repository>         prach_context_repo,
                   std::shared_ptr<uplink_context_repository>        ul_slot_context_repo,
                   std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo,
                   std::shared_ptr<uplink_cplane_context_repository> prach_cp_context_repo);

} // namespace ofh
} // namespace srsran
