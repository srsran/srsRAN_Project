
#include "../../lib/du_high/du_high.h"
#include "../../lib/f1_interface/f1ap_asn1_packer.h"
#include "../../lib/gateway/sctp_network_gateway.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Test F1 setup over "local" connection to DU.
void test_f1_setup_local()
{
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

  dummy_f1c_to_du_relay relay;
  dummy_f1c_pdu_handler pdu_handler(relay);
  du_high               du_obj(pdu_handler);

  du_obj.start();
}

/// Test F1 setup over network connection.
/// TODO: This is a component (or even integration) test by definition!!
void test_f1_setup_network()
{
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

  dummy_f1c_pdu_handler pdu_handler;
  du_high               du_obj(pdu_handler);

  du_obj.start();
}

void test_du_ue_create()
{
  class dummy_f1c_pdu_handler : public f1c_pdu_handler
  {
  public:
    asn1::f1ap::f1_ap_pdu_c last_pdu;
    void                    handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override { last_pdu = pdu; }
  };

  dummy_f1c_pdu_handler pdu_handler;
  du_high               du_obj(pdu_handler);

  du_obj.start();

  // Add UE
  mac_rx_pdu_list lst;
  lst.push_back(mac_rx_pdu{0x4601, 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}});
  du_obj.push_pusch(mac_rx_data_indication{slot_point{0, 0}, 0, std::move(lst)});

  for (uint32_t count = 0; count < 10000; count++) {
    if (pdu_handler.last_pdu.type() != asn1::f1ap::f1_ap_pdu_c::types_opts::nulltype) {
      break;
    }
    usleep(1000);
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
