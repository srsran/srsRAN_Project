
#include "../../lib/du_high/du_high.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_du_ue_create()
{
  du_high du_obj;

  du_obj.start();

  // Add UE
  mac_rx_data_indication rx_ind{slot_point{0, 0}, 0, {{0x4601, 0, 15, 0, 20, byte_buffer{}}}};
  du_obj.push_pusch(rx_ind);

  uint32_t count = 0;
  for (std::string s = du_obj.query("ues"); std::stoi(s) == 0 and count < 10000; s = du_obj.query("ues"), count++) {
    usleep(100);
  }
  TESTASSERT(std::stoi(du_obj.query("ues")) > 0);
}

int main()
{
  srslog::init();

  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("F1AP").set_level(srslog::basic_levels::info);

  test_du_ue_create();
}
