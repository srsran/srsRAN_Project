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

#include "srsran/phy/upper/channel_processors/pbch_encoder.h"
#include "srsran/phy/upper/channel_processors/pbch_modulator.h"
#include "srsran/phy/upper/channel_processors/ssb_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsran/phy/upper/signal_processors/pss_processor.h"
#include "srsran/phy/upper/signal_processors/sss_processor.h"

namespace srsran {

struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

/// Implements a parameter validator for \ref ssb_processor_impl.
class ssb_processor_validator_impl : public ssb_pdu_validator
{
public:
  // See interface for documentation.
  bool is_valid(const ssb_processor::pdu_t& pdu) const override { return true; }
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
  ssb_processor_impl(ssb_processor_config config) :
    encoder(std::move(config.encoder)),
    modulator(std::move(config.modulator)),
    dmrs(std::move(config.dmrs)),
    pss(std::move(config.pss)),
    sss(std::move(config.sss))
  {
    // Do nothing
  }

  void process(resource_grid_writer& grid, const pdu_t& pdu) override;
};

} // namespace srsran
