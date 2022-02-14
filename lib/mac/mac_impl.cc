
#include "mac_impl.h"
#include <mutex>

namespace srsgnb {

mac_ue_map::mac_ue_map()
{
  std::fill(rnti_to_ue_index_map.begin(), rnti_to_ue_index_map.end(), INVALID_RNTI);
}

mac_ue* mac_ue_map::find_by_rnti(rnti_t rnti)
{
  srsran_assert(rnti != INVALID_RNTI, "Invalid rnti=0x%x", rnti);
  du_ue_index_t ue_index = rnti_to_ue_index_map[rnti];
  if (ue_index == MAX_NOF_UES) {
    return nullptr;
  }
  return find(ue_index);
}

mac_ue* mac_ue_map::find(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
  element* e = &ue_db[ue_index];
  return e->present ? &e->ue : nullptr;
}

bool mac_ue_map::insert(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
  if (rnti_to_ue_index_map[crnti] < MAX_NOF_UES) {
    // rnti already exists
    return false;
  }

  bool prev_val = std::exchange(ue_db[ue_index].present, true);
  if (prev_val) {
    // UE already existed with same ue_index
    return false;
  }

  // Create UE object
  ue_db[ue_index].ue.rnti        = crnti;
  ue_db[ue_index].ue.du_ue_index = ue_index;
  ue_db[ue_index].ue.pcell_idx   = cell_index;

  rnti_to_ue_index_map[crnti] = ue_index;
  return true;
}

bool mac_ue_map::erase(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);

  bool prev_val = std::exchange(ue_db[ue_index].present, false);
  if (not prev_val) {
    // no UE existed with provided ue_index
    return false;
  }
  srsran_sanity_check(ue_db[ue_index].ue.rnti != INVALID_RNTI, "ue_index=%d has invalid RNTI", ue_index);
  rnti_to_ue_index_map[ue_db[ue_index].ue.rnti] = MAX_NOF_UES;
  ue_db[ue_index].ue                            = {};

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class sched_response_adapter final : public sched_cfg_notifier
{
public:
  sched_response_adapter(mac_impl& mac_) : mac_ref(mac_) {}

  void on_ue_config_complete(rnti_t rnti) override { mac_ref.sched_ue_config_response(rnti); }
  void on_ue_delete_response(rnti_t rnti) override { mac_ref.sched_ue_delete_response(rnti); }

  mac_impl& mac_ref;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

mac_impl::mac_impl(mac_config_notifier&  cfg_notifier_,
                   mac_ul_ccch_notifier& ul_ccch_notifier_,
                   task_executor&        ul_exec_,
                   span<task_executor*>  dl_execs_,
                   task_executor&        ctrl_exec_) :
  logger(srslog::fetch_basic_logger("MAC")),
  sched_notifier(std::make_unique<sched_response_adapter>(*this)),
  sched_obj(*sched_notifier),
  ctxt(cfg_notifier_, ul_exec_, dl_execs_, ctrl_exec_, sched_obj),
  ul_ccch_notifier(ul_ccch_notifier_)
{}

void mac_impl::push_ul_pdu(rnti_t rnti, du_cell_index_t cell_index, byte_buffer pdu)
{
  lcid_t lcid = 0;

  if (lcid == 0) {
    logger.info("UL\t0x%x\tcc=%d\tCCCH", rnti, cell_index);
    ul_ccch_notifier.on_ul_ccch_sdu(rnti, pdu);
  } else {
    mac_ul_dcch_notifier* dcch = ctxt.demux.get_rlc_bearer(rnti, lcid);
    if (dcch == nullptr) {
      logger.warning("Received UL PDU for inexistent bearer {0x%x, %d}", rnti, lcid);
      return;
    }

    // send PDU to RLC entity
    dcch->on_ul_dcch_sdu(pdu);
  }
}

void mac_impl::ue_create_request(const mac_ue_create_request_message& cfg)
{
  ue_create_proc = std::make_unique<mac_ue_create_request_procedure>(ctxt, cfg);
}

void mac_impl::ue_delete_request(const mac_ue_delete_request_message& cfg)
{
  ue_delete_proc = std::make_unique<mac_ue_delete_procedure>(ctxt, cfg);
}

void mac_impl::sched_ue_config_response(rnti_t rnti)
{
  // TODO: fetch UE based on RNTI
  ue_create_proc->sched_ue_config_response();
}

void mac_impl::sched_ue_delete_response(rnti_t rnti)
{
  // TODO: fetch UE based on RNTI
  ue_delete_proc->sched_ue_delete_response();
}

void mac_impl::slot_indication(slot_point sl_tx, du_cell_index_t cc)
{
  {
    std::lock_guard<std::mutex> lock(dl_mutex);
    // synchronize sched
  }

  // for each cc, generate MAC DL SDUs
  mac_ue* u = ue_db.find_by_rnti(0x4601);
  if (u != nullptr) {
    byte_buffer pdu;

    // TODO: Call MAC SDU builder
  }
}

} // namespace srsgnb