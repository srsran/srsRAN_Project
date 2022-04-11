
#ifndef SRSGNB_SCHED_CONFIGURATOR_H
#define SRSGNB_SCHED_CONFIGURATOR_H

#include "cell_configuration.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/rrc_nr/serving_cell.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"

struct ssb_configuration;

/// \remark Refer to ssb-periodicityServingCell, TS 38.331
enum ssb_periodicity { ms5, ms10, ms20, ms40, ms80, ms160 };

namespace srsgnb {

/// Cell Configuration Request.
/// \remark See O-RAN WG8, Section 9.2.3.2.1, Table 9.18.
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

  /// NOTE: Structs below do not follow ORAN specs.
  /// Imported from mac_cell_configuration (NR Cell Configuration, O-RAN WG8, Section 9.2.1.1).
  carrier_configuration dl_carrier;
  ssb_configuration ssb_config;
};

/// UE Creation Request.
struct sched_ue_creation_request_message {
  du_ue_index_t   ue_index;
  rnti_t          crnti;
  du_cell_index_t pcell_index;
};

/// UE Reconfiguration Request.
struct sched_ue_reconfiguration_message {
  du_ue_index_t   ue_index;
  rnti_t          crnti;
  du_cell_index_t pcell_index;
};

/// UE Delete Request.
struct sched_ue_delete_message {
  du_ue_index_t   ue_index;
  rnti_t          crnti;
  du_cell_index_t pcell_index;
};

/// RACH indication Message.
/// \remark See ORAN WG8 9.2.3.2.12.
struct rach_indication_message {
  du_ue_index_t cell_index;
  rnti_t        crnti;
  unsigned      timing_info;
  slot_point    slot_rx;
  /// Index of the first OFDM Symbol where RACH was detected.
  unsigned symbol_index;
  unsigned frequency_index;
  unsigned preamble_id;
  unsigned timing_advance;
};

/// Interface to Add/Remove UEs and Cells.
class sched_configurator
{
public:
  virtual ~sched_configurator()                                                                      = default;
  virtual bool handle_cell_configuration_request(const cell_configuration_request_message& msg)      = 0;
  virtual void handle_rach_indication(const rach_indication_message& msg)                            = 0;
  virtual void handle_add_ue_request(const sched_ue_creation_request_message& ue_request)            = 0;
  virtual void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) = 0;
  virtual void handle_ue_delete_request(du_ue_index_t ue_index)                                      = 0;
};

/// Interface used by scheduler to notify MAC that a configuration is complete.
class sched_configuration_notifier
{
public:
  virtual ~sched_configuration_notifier()                    = default;
  virtual void on_ue_config_complete(du_ue_index_t ue_index) = 0;
  virtual void on_ue_delete_response(du_ue_index_t ue_index) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONFIGURATOR_H
