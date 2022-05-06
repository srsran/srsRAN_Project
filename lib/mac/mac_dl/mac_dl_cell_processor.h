
#ifndef SRSGNB_MAC_CELL_PROCESSOR_H
#define SRSGNB_MAC_CELL_PROCESSOR_H

#include "../mac_config.h"
#include "mac_dl_ue_manager.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/scheduler/mac_scheduler.h"
#include "ssb_assembler.h"

namespace srsgnb {

class mac_dl_cell_processor final : public mac_cell_slot_handler, public mac_cell_controller
{
public:
  mac_dl_cell_processor(mac_common_config_t&          cfg_,
                        const mac_cell_configuration& cell_cfg_,
                        mac_scheduler&              sched_,
                        mac_dl_ue_manager&            ue_mng);

  /// Starts configured cell.
  async_task<void> start() override;

  /// Stops configured cell.
  async_task<void> stop() override;

  /// \brief Processing of a new Slot Indication. This operation involves the following sub-tasks:
  /// - The scheduler generates the scheduling grants for the provided slot and cell.
  /// - The scheduling grants are passed to the PHY.
  /// - The MAC DL PDUs are generated and passed to the PHY as well.
  void handle_slot_indication(slot_point sl_tx) override;

  /// \brief Retrieve SSB-specific parameters.
  const ssb_assembler& get_ssb_configuration() const;

private:
  void handle_slot_indication_impl(slot_point sl_tx);

  /// Assemble struct that is going to be passed down to PHY with the DL scheduling result.
  /// \remark FAPI will use this struct to generate a DL_TTI.Request.
  void assemble_dl_sched_request(mac_dl_sched_result&   mac_res,
                                 slot_point             sl_tx,
                                 du_cell_index_t        cell_index,
                                 const dl_sched_result& dl_res);

  /// Assemble struct with the MAC PDUs of a given slot and cell that is going be passed to the PHY.
  /// \remark FAPI will use this struct to generate a Tx_Data.Request.
  void assemble_dl_data_request(mac_dl_data_result&    mac_data,
                                slot_point             sl_tx,
                                du_cell_index_t        cell_index,
                                const dl_sched_result& dl_res);

  mac_common_config_t&         cfg;
  srslog::basic_logger&        logger;
  const mac_cell_configuration cell_cfg;
  task_executor&               cell_exec;
  mac_cell_result_notifier&    phy_cell;
  /// ssb_helper: contains the SSB-specific parameters that are derived from those passed by the DU interface. These
  /// parameters are passed to the scheduler and also also to the PHY to generate the SSB PDU and PBCH payload.
  ssb_assembler ssb_helper;

  mac_scheduler&   sched_obj;
  mac_dl_ue_manager& ue_mng;

  /// Represents activation cell state.
  // Note: For now, cells start active.
  enum class cell_state { inactive, active } state = cell_state::active;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_PROCESSOR_H
