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

#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/phy/upper/signal_processors/channel_estimator/factories.h"
#include "srsran/phy/upper/signal_processors/pucch/dmrs_pucch_estimator.h"

namespace srsran {

class dmrs_pucch_estimator_factory
{
public:
  virtual ~dmrs_pucch_estimator_factory()                = default;
  virtual std::unique_ptr<dmrs_pucch_estimator> create() = 0;
};

std::shared_ptr<dmrs_pucch_estimator_factory>
create_dmrs_pucch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory>      prg_factory,
                                       std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory,
                                       std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory,
                                       std::shared_ptr<port_channel_estimator_factory>       ch_estimator_factory);

} // namespace srsran
