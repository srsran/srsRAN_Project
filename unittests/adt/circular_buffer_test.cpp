/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/circular_buffer.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_static_circular_buffer()
{
  {
    static_circular_buffer<int, 10> circ_buffer;

    // test empty container
    TESTASSERT_EQ(10, circ_buffer.max_size());
    TESTASSERT_EQ(0, circ_buffer.size());
    TESTASSERT(circ_buffer.empty() and not circ_buffer.full());
    TESTASSERT(circ_buffer.begin() == circ_buffer.end());

    // push until full
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == i and not circ_buffer.full());
      circ_buffer.push(i);
      TESTASSERT(not circ_buffer.empty());
    }
    TESTASSERT(circ_buffer.size() == 10 and circ_buffer.full());

    // test iterator
    int count = 0;
    for (int it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
    TESTASSERT(*circ_buffer.begin() == circ_buffer.top());

    // pop until empty
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == circ_buffer.max_size() - i and not circ_buffer.empty());
      TESTASSERT(circ_buffer.top() == (int)i);
      circ_buffer.pop();
    }
    TESTASSERT(circ_buffer.empty() and circ_buffer.size() == 0);

    // test iteration with wrap-around in memory
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      circ_buffer.push(i);
    }
    for (size_t i = 0; i < circ_buffer.max_size() / 2; ++i) {
      circ_buffer.pop();
    }
    circ_buffer.push(circ_buffer.max_size());
    circ_buffer.push(circ_buffer.max_size() + 1);
    TESTASSERT(circ_buffer.size() == circ_buffer.max_size() / 2 + 2);
    count = circ_buffer.max_size() / 2;
    for (int& it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
  }

  // TEST: move-only types
  using C = moveonly_test_object;
  {
    static_circular_buffer<C, 5> circbuffer;
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    TESTASSERT(circbuffer.full() and C::object_count() == 5);
    C c = std::move(circbuffer.top());
    TESTASSERT(circbuffer.full() and C::object_count() == 6);
    circbuffer.pop();
    TESTASSERT(not circbuffer.full() and C::object_count() == 5);

    static_circular_buffer<C, 5> circbuffer2(std::move(circbuffer));
    TESTASSERT(circbuffer.empty() and circbuffer2.size() == 4);
    TESTASSERT(C::object_count() == 5);
    circbuffer.push(C{});
    TESTASSERT(C::object_count() == 6);
    circbuffer = std::move(circbuffer2);
    TESTASSERT(C::object_count() == 5);
  }

  TESTASSERT(C::object_count() == 0);
}

void test_dyn_circular_buffer()
{
  {
    dyn_circular_buffer<int> circ_buffer(10);
    TESTASSERT(circ_buffer.max_size() == 10);
    TESTASSERT(circ_buffer.empty() and not circ_buffer.full() and circ_buffer.size() == 0);

    // push until full
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == i and not circ_buffer.full());
      circ_buffer.push(i);
      TESTASSERT(not circ_buffer.empty());
    }
    TESTASSERT(circ_buffer.size() == 10 and circ_buffer.full());

    // test iterator
    int count = 0;
    for (int it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
    TESTASSERT(*circ_buffer.begin() == circ_buffer.top());

    // pop until empty
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == circ_buffer.max_size() - i and not circ_buffer.empty());
      TESTASSERT(circ_buffer.top() == (int)i);
      circ_buffer.pop();
    }
    TESTASSERT(circ_buffer.empty() and circ_buffer.size() == 0);

    // test iteration with wrap-around in memory
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      circ_buffer.push(i);
    }
    for (size_t i = 0; i < circ_buffer.max_size() / 2; ++i) {
      circ_buffer.pop();
    }
    circ_buffer.push(circ_buffer.max_size());
    circ_buffer.push(circ_buffer.max_size() + 1);
    TESTASSERT(circ_buffer.size() == circ_buffer.max_size() / 2 + 2);
    count = circ_buffer.max_size() / 2;
    for (int& it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
  }

  // TEST: move-only types
  using C = moveonly_test_object;
  {
    dyn_circular_buffer<C> circbuffer(5);
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    TESTASSERT(circbuffer.full() and C::object_count() == 5);
    C c = std::move(circbuffer.top());
    TESTASSERT(circbuffer.full() and C::object_count() == 6);
    circbuffer.pop();
    TESTASSERT(not circbuffer.full() and C::object_count() == 5);

    dyn_circular_buffer<C> circbuffer2(std::move(circbuffer));
    TESTASSERT(circbuffer.empty() and circbuffer2.size() == 4);
    TESTASSERT(C::object_count() == 5);
    circbuffer.set_size(5);
    circbuffer.push(C{});
    TESTASSERT(C::object_count() == 6);
    circbuffer = std::move(circbuffer2);
    TESTASSERT(C::object_count() == 5);
  }
  TESTASSERT(C::object_count() == 0);

  // TEST: copy-only types
  using D = copyonly_test_object;
  {
    dyn_circular_buffer<D> circbuffer(3);
    D                      d{};
    circbuffer.push(d);
    circbuffer.push(d);
    circbuffer.push(d);
    TESTASSERT(circbuffer.full() and D::object_count() == 4);

    dyn_circular_buffer<D> circbuffer2(circbuffer);
    TESTASSERT(circbuffer2.full() and circbuffer.full());
    TESTASSERT(D::object_count() == 7);
    circbuffer.pop();
    circbuffer.pop();
    TESTASSERT(D::object_count() == 5);
    circbuffer = circbuffer2;
    TESTASSERT(D::object_count() == 7);
  }
  TESTASSERT(D::object_count() == 0);
}

void test_circular_buffer_iterator()
{
  std::vector<int>         vals = {0, 1, 2, 3};
  dyn_circular_buffer<int> q(4);

  for (int v : vals) {
    q.push(v);
  }

  // Status: Queue is full.
  TESTASSERT(q.full());
  unsigned count = 0;
  for (int v : q) {
    TESTASSERT_EQ(vals[count++], v);
  }
  TESTASSERT_EQ(vals.size(), count);
}

void test_queue_block_api()
{
  dyn_blocking_queue<int> queue(100);

  std::thread t([&queue]() {
    int count = 0;
    while (true) {
      int val = queue.pop_blocking();
      if (queue.is_stopped()) {
        break;
      }
      TESTASSERT_EQ(val, count);
      count++;
    }
  });

  for (int i = 0; i < 10000; ++i) {
    queue.push_blocking(i);
  }

  queue.stop();
  t.join();
}

void test_queue_block_api_2()
{
  std::thread t;

  dyn_blocking_queue<int> queue(100);

  t = std::thread([&queue]() {
    int count = 0;
    while (queue.push_blocking(count++)) {
    }
  });

  for (int i = 0; i < 10000; ++i) {
    TESTASSERT(queue.pop_blocking() == i);
  }

  queue.stop();
  t.join();
}

int main()
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  test_static_circular_buffer();
  test_dyn_circular_buffer();
  test_circular_buffer_iterator();
  test_queue_block_api();
  test_queue_block_api_2();
  fmt::print("Success\n");
  return 0;
}
