
#include "units/du/du.h"

using namespace srsgnb;

void test_du_ue_create()
{
  du du_obj;

  du_obj.start();

  du_obj.push_pusch(0x4601, 0, byte_buffer{});

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