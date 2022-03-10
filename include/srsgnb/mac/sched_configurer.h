
#ifndef SRSGNB_SCHED_CONFIGURER_H
#define SRSGNB_SCHED_CONFIGURER_H

#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/rrc_nr.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Cell Configuration Request
struct cell_configuration_request_message {
  du_cell_index_t cell_index;
  uint8_t         nof_beams;     // (0..64)
  uint8_t         nof_layers;    // (0..8)
  uint8_t         nof_ant_ports; // (0..64)
  pci_t           pci;
  unsigned        dl_cell_bandwidth;
  unsigned        ul_cell_bandwidth;

  asn1::rrc_nr::dl_cfg_common_sib_s              dl_cfg_common;
  asn1::rrc_nr::ul_cfg_common_sib_s              ul_cfg_common;
  optional<asn1::rrc_nr::tdd_ul_dl_cfg_common_s> tdd_ul_dl_cfg_common; // absent == FDD
  // TODO: ...
};

/// Add UE Configuration Request
struct add_ue_configuration_request_message {};

/// RACH indication Message
struct rach_indication_message {
  du_ue_index_t cell_index;
  rnti_t        crnti;
  unsigned      timing_info;
  slot_point    slot_rx;
  /// Index of the first OFDM Symbol where RACH was detected
  unsigned symbol_index;
  unsigned frequency_index;
  unsigned preamble_id;
  unsigned timing_advance;
};

/// Interface to Add/Remove UEs and Cells
class sched_configurer
{
public:
  virtual ~sched_configurer()                                                                   = default;
  virtual bool handle_cell_configuration_request(const cell_configuration_request_message& msg) = 0;
  virtual void handle_rach_indication(const rach_indication_message& msg)                       = 0;
  virtual void config_ue(rnti_t rnti)                                                           = 0;
  virtual void delete_ue_request(rnti_t rnti)                                                   = 0;
};

/// Interface used by scheduler to notify MAC
class sched_cfg_notifier
{
public:
  virtual ~sched_cfg_notifier()                   = default;
  virtual void on_ue_config_complete(rnti_t)      = 0;
  virtual void on_ue_delete_response(rnti_t rnti) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONFIGURER_H
