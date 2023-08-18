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
#include "srsran/phy/upper/channel_processors/uci_decoder.h"

namespace srsran {

class pusch_uci_decoder_notifier
{
public:
  virtual ~pusch_uci_decoder_notifier() = default;

  virtual void on_uci_decoded(span<const uint8_t> message, const uci_status& status) = 0;
};

} // namespace srsran