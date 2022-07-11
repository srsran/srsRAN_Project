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

#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pbch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include "srsgnb/phy/upper/signal_processors/sss_processor.h"

namespace srsgnb {

struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

class ssb_processor_impl : public ssb_processor
{
private:
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;

public:
  ssb_processor_impl(ssb_processor_config& config) :
    encoder(std::move(config.encoder)),
    modulator(std::move(config.modulator)),
    dmrs(std::move(config.dmrs)),
    pss(std::move(config.pss)),
    sss(std::move(config.sss))
  {
    // Do nothing
  }

  void process(const pdu_t& pdu, resource_grid_writer& grid) override;
};

std::unique_ptr<ssb_processor> create_ssb_processor(ssb_processor_config& config);

} // namespace srsgnb
