
#include "../../../lib/mac/sched/sched_harq.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_harq()
{
  dl_harq_proc h{0, 52};
  TESTASSERT(h.empty());
}

int main()
{
  test_harq();
}