
#ifndef SRSGNB_RLC_H
#define SRSGNB_RLC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/common/id_types.h"

namespace srsgnb {

/********************************
 * 9.2.5.1 - UE Create
 *******************************/

struct rlc_ue_create_message {
  struct lc_ch_to_setup {
    lcid_t lcid;
    enum class rlc_mode { AM, UM } mode;
    // ...
  };

  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
};

struct rlc_ue_create_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

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
 * 9.2.5.5 - UE Delete
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
 * 9.2.5.11 - UE Reestablishment Request
 ***************************************/

struct rlc_ue_reestablishment_message {
  du_cell_index_t       cell_index;
  du_ue_index_t         ue_index;
  std::vector<uint32_t> lc_ch_ids;
};

struct rlc_ue_reestablishment_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

class rlc_config_interface
{
public:
  virtual void ue_create(const rlc_ue_create_message& cfg)                           = 0;
  virtual void start_ue_reconfiguration(const rlc_ue_reconfiguration_message& cfg)   = 0;
  virtual void start_ue_delete(const rlc_ue_delete_message& cfg)                     = 0;
  virtual void ue_reestablishment_request(const rlc_ue_reestablishment_message& cfg) = 0;
};

class rlc_interface : public rlc_config_interface
{
public:
  virtual ~rlc_interface() = default;
};

class rlc_config_notifier
{
public:
  virtual void ue_create_response(const rlc_ue_create_response_message& resp)                   = 0;
  virtual void ue_reconfiguration_response(const rlc_ue_reconfiguration_response_message& resp) = 0;
  virtual void ue_delete_response(const rlc_ue_delete_response_message& resp)                   = 0;
  virtual void ue_reestablishment_response(const rlc_ue_reestablishment_response_message& resp) = 0;
};

class ul_ccch_pdu_notifier
{
public:
  virtual void handle_pdu(const byte_buffer& pdu);
};

} // namespace srsgnb

#endif // SRSGNB_RLC_H
