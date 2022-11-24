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

#include "srsgnb/gtpu/gtpu_demux.h"

namespace srsgnb {

class dummy_ngu : public gtpu_demux_ctrl
{
public:
  dummy_ngu()  = default;
  ~dummy_ngu() = default;

  bool add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel) override { return true; }

  bool remove_tunnel(uint32_t teid) override { return true; }
};

} // namespace srsgnb
