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

#include "srsgnb/f1u/common/f1u_connector.h"
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

class dummy_f1u_connector final : public f1u_cu_up_connector
{
public:
  dummy_f1u_connector()  = default;
  ~dummy_f1u_connector() = default;

  void
  attach_cu_dl_bearer(uint32_t dl_teid, f1u_dl_local_adapter& cu_tx, srs_cu_up::f1u_rx_pdu_handler& cu_rx) override{};
  void attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid) override{};
};
} // namespace srsgnb
