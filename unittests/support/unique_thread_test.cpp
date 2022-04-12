
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/support/unique_thread.h"
#include <mutex>
#include <unistd.h>

using namespace srsgnb;

void test_uninit_unique_thread()
{
  test_delimit_logger delimit{"Uninitialized unique thread"};

  unique_thread t("T1");
  TESTASSERT(not t.running());
  TESTASSERT_EQ(std::string("T1"), t.get_name());
  TESTASSERT_NEQ(std::string(this_thread_name()), t.get_name());

  t.print_priority();

  // Join is a NO-OP.
  t.join();
  TESTASSERT(not t.running());
}

void test_init_unique_thread()
{
  test_delimit_logger delimit{"Initialized unique thread"};

  unique_thread t("T1");
  TESTASSERT(not t.running());

  // Action: start thread.
  std::atomic<bool> running{true};
  std::string       t_name;
  t.start([&running]() {
    TESTASSERT_EQ(std::string("T1"), this_thread_name());

    print_this_thread_priority();

    fmt::print("Thread [{}:{}] completed.\n", std::this_thread::get_id(), this_thread_name());
    running = false;
  });
  TESTASSERT_EQ(std::string(t.get_name()), "T1");
  TESTASSERT_NEQ(std::string(t.get_name()), this_thread_name());

  while (running) {
    usleep(100);
  }
  fmt::print("Main Thread [{}:{}] completed.\n", std::this_thread::get_id(), this_thread_name());
}

// Note: The thread priority is only set if this thread runs in sudo mode.
void test_init_unique_thread_prio()
{
  test_delimit_logger delimit{"Initialized unique thread with priority"};

  unique_thread t("T1");
  t.start(4, []() { print_this_thread_priority(); });
}

int main()
{
  test_uninit_unique_thread();
  test_init_unique_thread();
  test_init_unique_thread_prio();
}