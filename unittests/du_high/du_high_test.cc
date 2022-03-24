
#include "../../lib/du_high/du_high.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

struct f1_dummy_gateway : public f1c_du_gateway {
public:
  byte_buffer last_pdu;

  void on_new_sdu(byte_buffer pdu) override { last_pdu = std::move(pdu); }
};

void test_du_ue_create()
{
  f1_dummy_gateway gw;
  du_high          du_obj(gw);

  du_obj.start();

  // Add UE
  mac_rx_data_indication rx_ind{
      slot_point{0, 0},
      0,
      {mac_rx_pdu{0x4601, 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}}}};
  du_obj.push_pusch(rx_ind);

  for (uint32_t count = 0; count < 10000; count++) {
    if (not gw.last_pdu.empty()) {
      break;
    }
    usleep(1000);
  }
  TESTASSERT(not gw.last_pdu.empty());
}

int main()
{
  srslog::init();

  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("F1AP").set_level(srslog::basic_levels::info);

  test_du_ue_create();
}
