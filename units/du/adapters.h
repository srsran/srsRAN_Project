
#ifndef SRSGNB_ADAPTERS_H
#define SRSGNB_ADAPTERS_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

// class du_ccch_pdu_handler : public ul_ccch_pdu_notifier
//{
// public:
//   void handle_pdu(const byte_buffer& pdu) override
//   {
//     printf("[RLC-DUMANAGER-ADAPTER] Received a packet from F1u layer, forwarding data packet of size = %u bytes to "
//            "upper layer (PDCP)\n",
//            (unsigned)pdu.size());
//     du_manager.ul_ccch_indication(pdu);
//   }
//
// private:
//   du_manager_input_gateway& du_manager;
// };

/**
 * Adapter of RLC configuration notifications to DU manager method calls
 */
class rlc_cfg_adapter : public rlc_config_notifier
{
public:
  void connect(du_manager_interface& du_manager_) { du_manager = &du_manager_; }

  void ue_create_response(const rlc_ue_create_response_message& resp) override
  {
    du_manager->rlc_ue_create_response(resp);
  }
  void ue_reconfiguration_response(const rlc_ue_reconfiguration_response_message& resp) override
  {
    du_manager->rlc_ue_reconfiguration_response(resp);
  }
  void ue_delete_response(const rlc_ue_delete_response_message& resp) override
  {
    du_manager->rlc_ue_delete_response(resp);
  }
  void ue_reestablishment_response(const rlc_ue_reestablishment_response_message& resp) override {}

private:
  du_manager_interface* du_manager = nullptr;
};

/**
 * Adapter of MAC configuration notifications to DU manager method calls
 */
class mac_cfg_adapter : public mac_config_notifier
{
public:
  void connect(du_manager_interface& du_manager_) { du_manager = &du_manager_; }

  void ue_create_request_complete(const mac_ue_create_request_response_message& resp) override
  {
    du_manager->mac_ue_create_response(resp);
  }
  void ue_reconfiguration_complete() override {}
  void ue_delete_complete() override {}

private:
  du_manager_interface* du_manager = nullptr;
};

class mac_northbound_adapter : public mac_northbound_notifier
{
public:
  void connect(f1ap_du_ul_interface& f1ap_) { f1ap = &f1ap_; }

  void push_sdu(rnti_t rnti, lcid_t lcid, byte_buffer pdu)
  {
    ul_ccch_indication_message ul_ccch_msg;
    ul_ccch_msg.crnti      = rnti;
    ul_ccch_msg.cell_index = 0;
    f1ap->ul_ccch_message_indication(std::move(ul_ccch_msg));
  }

private:
  f1ap_du_ul_interface* f1ap = nullptr;
};

class f1ap_du_rlc_connector : public f1ap_du_pdu_notifier
{
  void push_pdu(const byte_buffer& data) override {}
};

class du_f1ap_cfg_adapter : public du_manager_config_notifier
{
public:
  void connect(f1ap_du_config_interface& f1ap_) { f1ap = &f1ap_; }

  void du_ue_create_response(const du_ue_create_response_message& resp) { f1ap->ue_creation_response(resp); }

private:
  f1ap_du_config_interface* f1ap = nullptr;
};

} // namespace srsgnb

#endif // SRSGNB_ADAPTERS_H
