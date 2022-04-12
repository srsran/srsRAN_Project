/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/unique_thread.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include <cstdio>
#include <pthread.h>
#include <sys/types.h>

using namespace srsgnb;

static bool thread_set_param(pthread_t t, int prio_offset)
{
  sched_param param{};

  int policy = 0;
  // All threads have normal priority except prio_offset=0,1,2,3,4
  if (prio_offset >= 0 && prio_offset < 5) {
    // Subtract one to the priority offset to avoid scheduling threads with the highest priority that could contend with
    // OS critical tasks.
    param.sched_priority = sched_get_priority_max(SCHED_FIFO) - prio_offset - 1;
    policy               = SCHED_FIFO;
  } else {
    param.sched_priority = 0;
    policy               = SCHED_OTHER;
  }

  if (pthread_setschedparam(t, policy, &param) != 0) {
    fprintf(stderr,
            "Warning: Scheduling priority of thread \"%s\" not changed. Cause: Not enough privileges.\n",
            this_thread_name());
    return false;
  }
  return true;
}

static bool thread_set_affinity(pthread_t t, int cpu)
{
  cpu_set_t cpuset;
  if (cpu > 0) {
    if (cpu > 50) {
      uint32_t mask;
      mask = cpu / 100;

      CPU_ZERO(&cpuset);
      for (uint32_t i = 0; i < 8; i++) {
        if (((mask >> i) & 0x01U) == 1U) {
          CPU_SET((size_t)i, &cpuset);
        }
      }
    } else {
      CPU_ZERO(&cpuset);
      CPU_SET((size_t)cpu, &cpuset);
    }

    if (pthread_setaffinity_np(t, sizeof(cpu_set_t), &cpuset) != 0) {
      perror("pthread_setaffinity_np");
      return false;
    }
  }
  return true;
}

static std::string compute_this_thread_name()
{
  // See Posix pthread_setname_np.
  const uint32_t  MAX_THREAD_NAME_LEN       = 16;
  char            name[MAX_THREAD_NAME_LEN] = {};
  const pthread_t tid                       = pthread_self();
  if (pthread_getname_np(tid, name, MAX_THREAD_NAME_LEN)) {
    perror("Could not get pthread name");
  }
  return std::string(name);
}

static void print_thread_priority(pthread_t t, const char* tname, std::thread::id tid)
{
  if (t == 0) {
    printf("Error: Trying to print priority of invalid thread handle\n");
    return;
  }

  cpu_set_t          cpuset;
  struct sched_param param;
  int                policy;
  const char*        p;
  int                s, j;

  s = pthread_getaffinity_np(t, sizeof(cpu_set_t), &cpuset);
  if (s != 0) {
    printf("error pthread_getaffinity_np: %s\n", strerror(s));
  }

  printf("Set returned by pthread_getaffinity_np() contained:\n");
  for (j = 0; j < CPU_SETSIZE; j++) {
    if (CPU_ISSET(j, &cpuset)) {
      printf("    CPU %d\n", j);
    }
  }

  s = pthread_getschedparam(t, &policy, &param);
  if (s != 0) {
    printf("error pthread_getaffinity_np: %s\n", strerror(s));
  }

  switch (policy) {
    case SCHED_FIFO:
      p = "SCHED_FIFO";
      break;
    case SCHED_RR:
      p = "SCHED_RR";
      break;
    default:
      p = "Other";
      break;
  }

  fmt::print("Thread [{}:{}]: Sched policy is \"{}\". Priority is {}.\n", tname, tid, p, param.sched_priority);
}

void unique_thread::print_priority()
{
  print_thread_priority(thread_handle.native_handle(), name.c_str(), thread_handle.get_id());
}

///////////////////////////////////////

bool unique_thread::start_impl(int cpu, int prio_offset, unique_function<void()> callable)
{
  srsran_assert(not running(), "Trying to start thread {} that is already running.", get_name());

  // Launch thread.
  thread_handle = std::thread([this, cpu, prio_offset, c = std::move(callable)]() {
    // Note: At this point, thread_handle.native_handle() is not yet set. pthread_self() is used instead.
    pthread_t tself = pthread_self();
    if (pthread_setname_np(tself, name.c_str()) != 0) {
      perror("pthread_setname_np");
      fmt::print("Thread [{}]: Error while setting thread name.", std::this_thread::get_id());
    }

    // TSAN seems to have issues with thread attributes when running as normal user, disable them in that case.
#ifndef HAVE_TSAN
    if (prio_offset != -1) {
      thread_set_param(tself, prio_offset);
    }
    if (cpu != -1) {
      thread_set_affinity(tself, cpu);
    }
#endif
    c();
  });

  return true;
}

const char* srsgnb::this_thread_name()
{
  /// Storage of current thread name, set via unique_thread.
  thread_local std::string this_thread_name_val = compute_this_thread_name();
  return this_thread_name_val.c_str();
}

void srsgnb::print_this_thread_priority()
{
  return print_thread_priority(pthread_self(), this_thread_name(), std::this_thread::get_id());
}
