
#include "ue.h"

using namespace srsgnb;

void ue::handle_sr_indication(const sr_indication_message& msg)
{
  last_sr = msg;
}
