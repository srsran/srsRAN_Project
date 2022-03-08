
#ifndef SRSGNB_SCHED_INTERFACE_H
#define SRSGNB_SCHED_INTERFACE_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Maximum grants per slot. Implementation-specific.
const size_t MAX_GRANTS = 16;
/// Maximum Logical channels per TB. Implementation-specific.
const size_t MAX_LC_GRANTS = 4;
const size_t MAX_NOF_TB    = 2;

enum class duplex_mode_t { TDD, FDD };

struct cell_configuration_request_message {
  du_cell_index_t cell_index;
  uint8_t         nof_beams;     // (0..64)
  uint8_t         nof_layers;    // (0..8)
  uint8_t         nof_ant_ports; // (0..64)
  pci_t           pci;
  duplex_mode_t   duplex_mode;
  unsigned        dl_cell_bandwidth;
  unsigned        ul_cell_bandwidth;
  // TODO: downlinkConfigCommon
  // TODO: tdd-UL-DL-ConfigurationCommon
  // TODO: ...
};

struct add_ue_configuration_request {
  du_cell_index_t cell_index;
  rnti_t          crnti;
  // TODO: fill remaining fields
};

struct pdcch_config {};

struct pdsch_config {};

struct dl_msg_lc_info {
  /// LCID {0..32}
  lcid_t lcid;
  /// Number of scheduled bytes for this specific logical channel. {0..65535}
  unsigned sched_bytes;
};

struct dl_msg_tb_info {
  /// List of allocated logical channels
  static_vector<dl_msg_lc_info, MAX_LC_GRANTS> lc_lst;
};

/// Dedicated DL Grant for UEs
struct dl_msg_alloc {
  rnti_t                                    crnti;
  static_vector<dl_msg_tb_info, MAX_NOF_TB> tbs;
};

struct dl_sched_result {
  pci_t      cell_id;
  slot_point slot_value;

  /// Allocation of dedicated UE messages
  static_vector<dl_msg_alloc, MAX_GRANTS> ue_grants;
};

struct ul_sched_info {};

using ul_sched_result = static_vector<ul_sched_info, MAX_GRANTS>;

/// Interface to Add/Remove UEs and Cells
class sched_configurer
{
public:
  virtual ~sched_configurer()                 = default;
  virtual void config_ue(rnti_t rnti)         = 0;
  virtual void delete_ue_request(rnti_t rnti) = 0;
};

class sched_interface : public sched_configurer
{
public:
  virtual const dl_sched_result* get_dl_sched(slot_point sl_tx, du_cell_index_t cc) = 0;
  virtual const ul_sched_result* get_ul_sched(slot_point sl_tx, du_cell_index_t cc) = 0;
};

class sched_cfg_notifier
{
public:
  virtual ~sched_cfg_notifier()                   = default;
  virtual void on_ue_config_complete(rnti_t)      = 0;
  virtual void on_ue_delete_response(rnti_t rnti) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_INTERFACE_H
