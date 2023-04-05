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
#include "downlink_processor_baseband_impl.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/lower/sampling_rate.h"

namespace srsran {

/// Implements a software generic lower PHY downlink processor.
class downlink_processor_impl : public lower_phy_downlink_processor
{
public:
  /// \brief Constructs a software generic lower PHY downlink processor that can process downlink resource grids.
  /// \param[in] pdxch_proc PDxCH processor.
  /// \param[in] config     Downlink processor configuration.
  downlink_processor_impl(std::unique_ptr<pdxch_processor>                 pdxch_proc,
                          const downlink_processor_baseband_configuration& config);

  // See interface for documentation.
  void connect(downlink_processor_notifier& notifier, pdxch_processor_notifier& pdxch_notifier) override;

  // See interface for documentation.
  pdxch_processor_request_handler& get_downlink_request_handler() override;

  // See interface for documentation.
  downlink_processor_baseband& get_baseband() override;

private:
  /// PDxCH processor.
  std::unique_ptr<pdxch_processor> pdxch_proc;
  /// Baseband processor.
  downlink_processor_baseband_impl downlink_proc_baseband;
};

} // namespace srsran
