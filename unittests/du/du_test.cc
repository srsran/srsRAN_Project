
#include "units/du/du_high.h"

using namespace srsgnb;

void test_du_ue_create()
{
  du_high du_obj;

  du_obj.start();

  // Add UE
  mac_rx_data_indication rx_ind{0x4601, 0, 0, byte_buffer{}};
  du_obj.push_pusch(rx_ind);

  for (std::string s = du_obj.query("ues"); std::stoi(s) == 0; s = du_obj.query("ues")) {
    usleep(100);
  }
}

int main()
{
  srslog::init();

  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("F1AP").set_level(srslog::basic_levels::info);

  test_du_ue_create();
}