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

/// \brief Describes the uplink processor interface.
///
/// The uplink processor is in charge of processing incoming requests to decode the physical uplink channels within a
/// certain slot.
class uplink_processor
{
public:
  virtual ~uplink_processor() = default;

  /// \brief Processes the PRACH using the given configuration and context.
  ///
  /// The PRACH detection results will be notified by the \c on_prach_results event notifier.
  ///
  /// \param[in] context Context used by the underlying PRACH detector.
  /// \param[in] configuration Configuration parameters to detect any potential PRACH preambles.
  virtual void process_prach(const prach_buffer_context&               context,
                             const prach_detector::slot_configuration& configuration) = 0;
};

} // namespace srsgnb
