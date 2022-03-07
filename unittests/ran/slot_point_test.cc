
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_slot_type()
{
  // TEST: constructors
  slot_point slot1;
  TESTASSERT(not slot1.valid());
  slot_point slot2{0, 1, 5};
  TESTASSERT(slot2.valid() and slot2.numerology_idx() == 0 and slot2.slot_idx() == 5 and slot2.slot_idx() == 5 and
             slot2.sfn() == 1);
  slot_point slot3{slot2};
  TESTASSERT(slot3 == slot2);

  // TEST: comparison and difference operators
  slot1 = slot_point{0, 1, 5};
  slot2 = slot_point{0, 1, 5};
  TESTASSERT(slot1 == slot2 and slot1 <= slot2 and slot1 >= slot2);
  slot1++;
  TESTASSERT(slot1 != slot2 and slot1 >= slot2 and slot1 > slot2 and slot2 < slot1 and slot2 <= slot1);
  TESTASSERT(slot1 - slot2 == 1 and slot2 - slot1 == -1);
  slot1 = slot_point{0, 2, 5};
  TESTASSERT(slot1 != slot2 and slot1 >= slot2 and slot1 > slot2 and slot2 < slot1 and slot2 <= slot1);
  TESTASSERT(slot1 - slot2 == 10 and slot2 - slot1 == -10);
  slot1 = slot_point{0, 1023, 5};
  TESTASSERT(slot1 != slot2 and slot1 <= slot2 and slot1 < slot2 and slot2 > slot1 and slot2 >= slot1);
  TESTASSERT(slot1 - slot2 == -20 and slot2 - slot1 == 20);

  // TEST: increment/decrement operators
  slot1 = slot_point{0, 1, 5};
  slot2 = slot_point{0, 1, 5};
  TESTASSERT(slot1++ == slot2);
  TESTASSERT(slot2 + 1 == slot1);
  TESTASSERT(++slot2 == slot1);
  slot1 = slot_point{0, 1, 5};
  slot2 = slot_point{0, 1, 5};
  TESTASSERT(slot1 - 100 == slot2 - 100);
  TESTASSERT(((slot1 - 100000) + 100000) == slot1);
  TESTASSERT((slot1 - 10240) == slot1);
  TESTASSERT((slot1 - 100).slot_idx() == 5 and (slot1 - 100).sfn() == 1015);
  TESTASSERT(((slot1 - 100) + 100) == slot1);
  TESTASSERT(((slot1 - 1) + 1) == slot1);

  TESTASSERT(fmt::format("{}", slot1) == fmt::format("{}", slot1.to_uint()));
}

int main()
{
  srslog::init();

  test_slot_type();

  return 0;
}
