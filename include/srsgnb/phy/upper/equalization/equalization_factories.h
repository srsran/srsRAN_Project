
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

#include "srsgnb/phy/upper/equalization/channel_equalizer.h"

namespace srsgnb {

class channel_equalizer_factory
{
public:
  virtual ~channel_equalizer_factory() = default;

  /// \brief Creates a channel equalizer.
  /// \return A unique pointer to a channel equalizer instance.
  virtual std::unique_ptr<channel_equalizer> create() = 0;
};

std::shared_ptr<channel_equalizer_factory> create_channel_equalizer_factory();

} // namespace srsgnb
