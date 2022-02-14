
#ifndef SRSGNB_RLC_H
#define SRSGNB_RLC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

/********************************
 * 9.2.5.3 - UE Reconfiguration
 *******************************/

struct rlc_ue_reconfiguration_message {
  struct lc_ch_to_setup {
    lcid_t lcid;
    enum class rlc_mode { AM, UM } mode;
    // ...
  };

  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
};

struct rlc_ue_reconfiguration_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

/********************************
 * ORAN WP8 9.2.5.5 - UE Delete
 *******************************/

struct rlc_ue_delete_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
};

struct rlc_ue_delete_response_message {
  du_cell_index_t cell_index;
  bool            result;
};

/****************************************
 * ORAN WP8 9.2.5.8 - UL RRC Message Transfer
 ***************************************/

struct ul_rrc_transfer_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  lcid_t          lcid;
  byte_buffer     rrc_msg;
};

/**************************************************
 * ORAN WP8 9.2.5.11 - UE Reestablishment Request
 *************************************************/

struct rlc_ue_reestablishment_message {
  du_cell_index_t     cell_index;
  du_ue_index_t       ue_index;
  std::vector<lcid_t> lc_ch_ids;
};

struct rlc_ue_reestablishment_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

class rlc_ul_bearer
{
public:
  virtual ~rlc_ul_bearer()                      = default;
  virtual void push_pdu(const byte_buffer& pdu) = 0;
};

class rlc_config_notifier
{
public:
  virtual ~rlc_config_notifier()                                                                   = default;
  virtual void on_ue_reconfiguration_complete(const rlc_ue_reconfiguration_response_message& resp) = 0;
  virtual void on_ue_delete_complete(const rlc_ue_delete_response_message& resp)                   = 0;
  virtual void on_ue_reestablishment_complete(const rlc_ue_reestablishment_response_message& resp) = 0;
};

class rlc_ul_sdu_notifier
{
public:
  virtual ~rlc_ul_sdu_notifier()                                               = default;
  virtual void on_ul_sdu(du_ue_index_t ue_index, lcid_t lcid, byte_buffer pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_H
