
#include "units/du/du.h"

using namespace srsgnb;

void test_du_ue_create()
{
  du du_obj;

  du_obj.push_pusch(0x4601, 0, byte_buffer{});
}

int main()
{
  test_du_ue_create();
}