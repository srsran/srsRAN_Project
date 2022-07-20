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

#include "srsgnb/rrc/rrc.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Interface class for an RRC UE object.
/// It will contain getters for the interfaces for the various logical channels handled by RRC.
class rrc_ue_entity_interface
{
public:
  rrc_ue_entity_interface()          = default;
  virtual ~rrc_ue_entity_interface() = default;

  virtual rrc_ul_ccch_pdu_handler* get_ul_ccch_pdu_handler() = 0;
};

/// Interface to notify the reception of an new SRB PDU.
class rrc_pdu_notifier
{
public:
  virtual ~rrc_pdu_notifier() = default;

  /// This callback is invoked on each received PDU.
  virtual void on_new_pdu(byte_buffer_slice pdu) = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
