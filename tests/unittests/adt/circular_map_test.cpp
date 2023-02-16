/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

void test_id_map()
{
  circular_map<uint32_t, std::string, 16> myobj;
  TESTASSERT_EQ(0, myobj.size());
  TESTASSERT(myobj.empty() and not myobj.full());
  TESTASSERT(myobj.begin() == myobj.end());

  TESTASSERT(not myobj.contains(0));
  TESTASSERT(myobj.insert(0, "obj0"));
  TESTASSERT(myobj.contains(0) and myobj[0] == "obj0");
  TESTASSERT_EQ(1, myobj.size());
  TESTASSERT(not myobj.empty() and not myobj.full());
  TESTASSERT(myobj.begin() != myobj.end());

  TESTASSERT(not myobj.insert(0, "obj0"));
  TESTASSERT(myobj.insert(1, "obj1"));
  TESTASSERT(myobj.contains(0) and myobj.contains(1) and myobj[1] == "obj1");
  TESTASSERT(myobj.size() == 2 and not myobj.empty() and not myobj.full());

  TESTASSERT(myobj.find(1) != myobj.end());
  TESTASSERT_EQ(1, myobj.find(1)->first);
  TESTASSERT(myobj.find(1)->second == "obj1");

  // TEST: iteration
  uint32_t count = 0;
  for (std::pair<uint32_t, std::string>& obj : myobj) {
    TESTASSERT(obj.second == "obj" + std::to_string(count++));
  }

  // TEST: const iteration
  count = 0;
  for (const std::pair<uint32_t, std::string>& obj : myobj) {
    TESTASSERT(obj.second == "obj" + std::to_string(count++));
  }

  TESTASSERT(myobj.erase(0));
  TESTASSERT(myobj.erase(1));
  TESTASSERT(myobj.size() == 0 and myobj.empty());

  TESTASSERT(myobj.insert(0, "obj0"));
  TESTASSERT(myobj.insert(1, "obj1"));
  TESTASSERT(myobj.size() == 2 and not myobj.empty() and not myobj.full());
  myobj.clear();
  TESTASSERT(myobj.size() == 0 and myobj.empty());
}

struct C {
  C() { count++; }
  ~C() { count--; }
  C(C&&) { count++; }
  C(const C&)       = delete;
  C& operator=(C&&) = default;

  static size_t count;
};
size_t C::count = 0;

void test_correct_destruction()
{
  TESTASSERT(C::count == 0);
  {
    circular_map<uint32_t, C, 4> circ_buffer;
    TESTASSERT(C::count == 0);
    TESTASSERT(circ_buffer.insert(0, C{}));
    TESTASSERT(C::count == 1);
    TESTASSERT(circ_buffer.insert(1, C{}));
    TESTASSERT(circ_buffer.insert(2, C{}));
    TESTASSERT(circ_buffer.insert(3, C{}));
    TESTASSERT(C::count == 4);
    TESTASSERT(not circ_buffer.insert(4, C{}));
    TESTASSERT(C::count == 4);
    TESTASSERT(circ_buffer.erase(1));
    TESTASSERT(C::count == 3);
    TESTASSERT(not circ_buffer.contains(1));

    std::array<uint32_t, 3> content{0, 2, 3};
    size_t                  i = 0;
    for (auto& e : circ_buffer) {
      TESTASSERT(content[i] == e.first);
      i++;
    }

    TESTASSERT(C::count == 3);
    circular_map<uint32_t, C, 4> circ_buffer2;
    circ_buffer2 = std::move(circ_buffer);
    TESTASSERT(C::count == 3);

    circular_map<uint32_t, C, 4> circ_buffer3;
    TESTASSERT(circ_buffer3.insert(1, C{}));
    TESTASSERT(C::count == 4);
    circ_buffer2 = std::move(circ_buffer3);
    TESTASSERT(C::count == 1);
  }
  TESTASSERT(C::count == 0);
}

} // namespace srsgnb

int main()
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  srsgnb::test_id_map();
  srsgnb::test_correct_destruction();

  printf("Success\n");
  return 0;
}
