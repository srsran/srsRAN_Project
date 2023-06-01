/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "downlink_processor_baseband_impl.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
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
  /// \param[in] pdxch_proc        PDxCH processor.
  /// \param[in] amplitude_control Amplitude controller.
  /// \param[in] config            Downlink processor configuration.
  downlink_processor_impl(std::unique_ptr<pdxch_processor>                 pdxch_proc,
                          std::unique_ptr<amplitude_controller>            amplitude_control,
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
  /// Amplitude control.
  std::unique_ptr<amplitude_controller> amplitude_control;
  /// Baseband processor.
  downlink_processor_baseband_impl downlink_proc_baseband;
};

} // namespace srsran
