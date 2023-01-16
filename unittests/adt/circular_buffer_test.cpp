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
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(circular_buffer_test, static_size)
{
  {
    static_circular_buffer<int, 10> circ_buffer;

    // test empty container
    ASSERT_EQ(10, circ_buffer.max_size());
    ASSERT_EQ(0, circ_buffer.size());
    ASSERT_TRUE(circ_buffer.empty());
    ASSERT_FALSE(circ_buffer.full());
    ASSERT_EQ(circ_buffer.begin(), circ_buffer.end());

    // push until full
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      ASSERT_EQ(circ_buffer.size(), i);
      ASSERT_FALSE(circ_buffer.full());
      circ_buffer.push(i);
      ASSERT_FALSE(circ_buffer.empty());
    }
    ASSERT_EQ(circ_buffer.size(), 10);
    ASSERT_TRUE(circ_buffer.full());

    // test iterator
    int count = 0;
    for (int it : circ_buffer) {
      ASSERT_EQ(it, count);
      count++;
    }
    ASSERT_EQ(*circ_buffer.begin(), circ_buffer.top());

    // pop until empty
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      ASSERT_EQ(circ_buffer.size(), circ_buffer.max_size() - i);
      ASSERT_FALSE(circ_buffer.empty());
      ASSERT_EQ(circ_buffer.top(), (int)i);
      circ_buffer.pop();
    }
    ASSERT_TRUE(circ_buffer.empty());
    ASSERT_EQ(circ_buffer.size(), 0);

    // test iteration with wrap-around in memory
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      circ_buffer.push(i);
    }
    for (size_t i = 0; i < circ_buffer.max_size() / 2; ++i) {
      circ_buffer.pop();
    }
    circ_buffer.push(circ_buffer.max_size());
    circ_buffer.push(circ_buffer.max_size() + 1);
    ASSERT_EQ(circ_buffer.size(), circ_buffer.max_size() / 2 + 2);
    count = circ_buffer.max_size() / 2;
    for (int& it : circ_buffer) {
      ASSERT_EQ(it, count);
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
    ASSERT_TRUE(circbuffer.full());
    ASSERT_EQ(C::object_count(), 5);
    C c = std::move(circbuffer.top());
    ASSERT_TRUE(circbuffer.full());
    ASSERT_EQ(C::object_count(), 6);
    circbuffer.pop();
    ASSERT_FALSE(circbuffer.full());
    ASSERT_EQ(C::object_count(), 5);

    static_circular_buffer<C, 5> circbuffer2(std::move(circbuffer));
    ASSERT_TRUE(circbuffer.empty());
    ASSERT_EQ(circbuffer2.size(), 4);
    ASSERT_EQ(C::object_count(), 5);
    circbuffer.push(C{});
    ASSERT_EQ(C::object_count(), 6);
    circbuffer = std::move(circbuffer2);
    ASSERT_EQ(C::object_count(), 5);
  }

  ASSERT_EQ(C::object_count(), 0);
}

TEST(circular_buffer_test, dynamic_size)
{
  {
    dyn_circular_buffer<int> circ_buffer(10);
    ASSERT_EQ(circ_buffer.max_size(), 10);
    ASSERT_TRUE(circ_buffer.empty());
    ASSERT_FALSE(circ_buffer.full());
    ASSERT_EQ(circ_buffer.size(), 0);

    // push until full
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      ASSERT_FALSE(circ_buffer.full());
      ASSERT_EQ(circ_buffer.size(), i);
      circ_buffer.push(i);
      ASSERT_FALSE(circ_buffer.empty());
    }
    ASSERT_TRUE(circ_buffer.full());
    ASSERT_EQ(circ_buffer.size(), 10);

    // test iterator
    int count = 0;
    for (int it : circ_buffer) {
      ASSERT_EQ(it, count);
      count++;
    }
    ASSERT_EQ(*circ_buffer.begin(), circ_buffer.top());

    // pop until empty
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      ASSERT_FALSE(circ_buffer.empty());
      ASSERT_EQ(circ_buffer.size(), circ_buffer.max_size() - i);
      ASSERT_EQ(circ_buffer.top(), (int)i);
      circ_buffer.pop();
    }
    ASSERT_TRUE(circ_buffer.empty());
    ASSERT_EQ(circ_buffer.size(), 0);

    // test iteration with wrap-around in memory
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      circ_buffer.push(i);
    }
    for (size_t i = 0; i < circ_buffer.max_size() / 2; ++i) {
      circ_buffer.pop();
    }
    circ_buffer.push(circ_buffer.max_size());
    circ_buffer.push(circ_buffer.max_size() + 1);
    ASSERT_EQ(circ_buffer.size(), circ_buffer.max_size() / 2 + 2);
    count = circ_buffer.max_size() / 2;
    for (int& it : circ_buffer) {
      ASSERT_EQ(it, count);
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
    ASSERT_TRUE(circbuffer.full());
    ASSERT_EQ(C::object_count(), 5);
    C c = std::move(circbuffer.top());
    ASSERT_TRUE(circbuffer.full());
    ASSERT_EQ(C::object_count(), 6);
    circbuffer.pop();
    ASSERT_FALSE(circbuffer.full());
    ASSERT_EQ(C::object_count(), 5);

    dyn_circular_buffer<C> circbuffer2(std::move(circbuffer));
    ASSERT_TRUE(circbuffer.empty());
    ASSERT_EQ(circbuffer2.size(), 4);
    ASSERT_EQ(C::object_count(), 5);
    circbuffer.set_size(5);
    circbuffer.push(C{});
    ASSERT_EQ(C::object_count(), 6);
    circbuffer = std::move(circbuffer2);
    ASSERT_EQ(C::object_count(), 5);
  }
  ASSERT_EQ(C::object_count(), 0);

  // TEST: copy-only types
  using D = copyonly_test_object;
  {
    dyn_circular_buffer<D> circbuffer(3);
    D                      d{};
    circbuffer.push(d);
    circbuffer.push(d);
    circbuffer.push(d);
    ASSERT_TRUE(circbuffer.full());
    ASSERT_EQ(D::object_count(), 4);

    dyn_circular_buffer<D> circbuffer2(circbuffer);
    ASSERT_TRUE(circbuffer2.full());
    ASSERT_TRUE(circbuffer.full());
    ASSERT_EQ(D::object_count(), 7);
    circbuffer.pop();
    circbuffer.pop();
    ASSERT_EQ(D::object_count(), 5);
    circbuffer = circbuffer2;
    ASSERT_EQ(D::object_count(), 7);
  }
  ASSERT_EQ(D::object_count(), 0);
}

TEST(circular_buffer_test, iterator)
{
  std::vector<int>         vals = {0, 1, 2, 3};
  dyn_circular_buffer<int> q(4);

  for (int v : vals) {
    q.push(v);
  }

  // Status: Queue is full.
  ASSERT_TRUE(q.full());
  unsigned count = 0;
  for (int v : q) {
    ASSERT_EQ(vals[count++], v);
  }
  ASSERT_EQ(vals.size(), count);
}

TEST(blocking_queue_test, api)
{
  dyn_blocking_queue<int> queue(100);

  std::thread t([&queue]() {
    int count = 0;
    while (true) {
      int val = queue.pop_blocking();
      if (queue.is_stopped()) {
        break;
      }
      ASSERT_EQ(val, count);
      count++;
    }
  });

  for (int i = 0; i < 10000; ++i) {
    queue.push_blocking(i);
  }

  queue.stop();
  t.join();
}

TEST(blocking_queue_test, api2)
{
  std::thread t;

  dyn_blocking_queue<int> queue(100);

  t = std::thread([&queue]() {
    int count = 0;
    while (queue.push_blocking(count++)) {
    }
  });

  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(queue.pop_blocking(), i);
  }

  queue.stop();
  t.join();
}
