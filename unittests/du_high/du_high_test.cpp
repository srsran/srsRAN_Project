
#include "../../lib/du_high/du_high.h"
#include "../../lib/f1_interface/f1ap_asn1_packer.h"
#include "../../lib/gateway/sctp_network_gateway.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

struct phy_cell_dummy : public mac_cell_result_notifier {
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override {}
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override {}
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override {}
};

struct phy_dummy : public mac_result_notifier {
public:
  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
  phy_cell_dummy            cell;
};

/// Test F1 setup over "local" connection to DU.
void test_f1_setup_local()
{
  test_delimit_logger delim{"Test F1 Setup Local"};

  class dummy_f1c_pdu_handler : public f1c_pdu_handler
  {
  public:
    dummy_f1c_pdu_handler(f1c_pdu_notifier& listener_) : listener(listener_) {}

    f1c_pdu_notifier&       listener;
    asn1::f1ap::f1_ap_pdu_c last_pdu;
    void                    handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override
    {
      // store and loop PDU back to sender
      last_pdu = pdu;
      listener.on_new_pdu(pdu);
    }
  };

  class dummy_f1c_to_du_relay : public srsgnb::f1c_pdu_notifier
  {
  public:
    void on_new_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override
    {
      std::printf("[F1C-TO_DU-RELAY] Received a F1AP PDU of type %s\n", pdu.type().to_string());
    }
  };

  const size_t          task_worker_queue_size = 10000;
  task_worker           ctrl_worker("DU-CTRL", task_worker_queue_size);
  task_worker_executor  ctrl_exec(ctrl_worker);
  dummy_f1c_to_du_relay relay;
  dummy_f1c_pdu_handler pdu_handler(relay);
  phy_dummy             phy;

  du_high_configuration cfg{};
  cfg.du_mng_executor = &ctrl_exec;
  cfg.f1c_pdu_hdl     = &pdu_handler;
  cfg.phy_adapter     = &phy;

  du_high du_obj(cfg);

  du_obj.start();
}

/// Test F1 setup over network connection.
/// TODO: This is a component (or even integration) test by definition!!
void test_f1_setup_network()
{
  test_delimit_logger delim{"Test F1 Setup Network"};

  class dummy_f1c_pdu_handler : public f1c_pdu_handler
  {
  public:
    dummy_f1c_pdu_handler() : packer(gw, *this) {}

    /// We require a network gateway and a packer
    sctp_network_gateway gw;
    f1ap_asn1_packer     packer;

    asn1::f1ap::f1_ap_pdu_c last_pdu;
    void                    handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override { last_pdu = pdu; }
  };

  const size_t          task_worker_queue_size = 10000;
  task_worker           ctrl_worker("DU-CTRL", task_worker_queue_size);
  task_worker_executor  ctrl_exec(ctrl_worker);
  dummy_f1c_pdu_handler pdu_handler;
  phy_dummy             phy;

  du_high_configuration cfg{};
  cfg.du_mng_executor = &ctrl_exec;
  cfg.f1c_pdu_hdl     = &pdu_handler;
  cfg.phy_adapter     = &phy;

  du_high du_obj(cfg);

  du_obj.start();
}

void test_du_ue_create()
{
  test_delimit_logger delim{"Test DU UE Create"};

  class dummy_f1c_pdu_handler : public f1c_pdu_handler
  {
  public:
    asn1::f1ap::f1_ap_pdu_c last_pdu;
    task_executor*          ctrl_exec;
    void                    handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override
    {
      ctrl_exec->execute([this, pdu]() { last_pdu = pdu; });
    }
  };

  const size_t          task_worker_queue_size = 10000;
  manual_task_worker    ctrl_worker{task_worker_queue_size};
  dummy_f1c_pdu_handler pdu_handler;
  pdu_handler.ctrl_exec = &ctrl_worker;
  phy_dummy phy;

  du_high_configuration cfg{};
  cfg.du_mng_executor = &ctrl_worker;
  cfg.f1c_pdu_hdl     = &pdu_handler;
  cfg.phy_adapter     = &phy;

  du_high du_obj(cfg);

  du_obj.start();

  // Add UE
  mac_rx_pdu_list lst;
  lst.push_back(mac_rx_pdu{to_rnti(0x4601), 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}});
  du_obj.get_pdu_handler(0).handle_rx_data_indication(mac_rx_data_indication{slot_point{0, 0}, 0, std::move(lst)});

  slot_point sl_tx{0, 0};
  for (uint32_t count = 0; count < 10000; count++) {
    du_obj.get_slot_handler(0).handle_slot_indication(sl_tx++);
    if (pdu_handler.last_pdu.type() != asn1::f1ap::f1_ap_pdu_c::types_opts::nulltype) {
      break;
    }
    ctrl_worker.run_next_blocking();
  }
  TESTASSERT(pdu_handler.last_pdu.type() != asn1::f1ap::f1_ap_pdu_c::types_opts::nulltype);
}

int main()
{
  srslog::init();

  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("F1AP").set_level(srslog::basic_levels::info);

  test_f1_setup_local();
  test_f1_setup_network();
  test_du_ue_create();
}
