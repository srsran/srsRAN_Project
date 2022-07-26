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

#include "srsgnb/phy/upper/channel_processors/prach_detector.h"

namespace srsgnb {

struct prach_buffer_context;

/// \brief Uplink processor that groups and process all the uplink channels within a slot.
class uplink_processor
{
public:
  virtual ~uplink_processor() = default;

  /// \brief Process PRACHs PDUs using the the given PRACH configuration and context.
  ///
  /// The PRACH detection results will be notified by \c
  ///
  /// \param[in] context Contains the context for the PRACH detection.
  /// \param[in] configuration Configuration parameters for the PRACH detector.
  virtual void process_prach(const prach_buffer_context&               context,
                             const prach_detector::slot_configuration& configuration) = 0;
};

} // namespace srsgnb
