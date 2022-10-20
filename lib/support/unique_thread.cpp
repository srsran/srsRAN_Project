/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

size_t srsgnb::compute_host_nof_hardware_threads()
{
  cpu_set_t cpuset;
  if (sched_getaffinity(0, sizeof(cpuset), &cpuset) == 0) {
    return std::max(1, CPU_COUNT(&cpuset));
  }
  return std::max(1U, std::thread::hardware_concurrency());
}

/// Sets thread OS scheduling real-time priority.
static bool thread_set_param(pthread_t t, os_thread_realtime_priority prio)
{
  sched_param param{};

  int policy = 0;
  if (prio != srsgnb::os_thread_realtime_priority::NO_REALTIME) {
    // Subtract one to the priority offset to avoid scheduling threads with the highest priority that could contend with
    // OS critical tasks.
    param.sched_priority = sched_get_priority_max(SCHED_FIFO) + (int)prio - 32;
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

static bool thread_set_affinity(pthread_t t, const os_sched_affinity_bitmask& bitmap)
{
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  for (size_t i = 0; i < bitmap.size(); ++i) {
    if (bitmap.test(i)) {
      CPU_SET(i, &cpuset);
    }
  }
  if (pthread_setaffinity_np(t, sizeof(cpu_set_t), &cpuset) != 0) {
    perror("pthread_setaffinity_np");
    return false;
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

std::thread unique_thread::make_thread(const std::string&               name,
                                       unique_function<void()>          callable,
                                       os_thread_realtime_priority      prio,
                                       const os_sched_affinity_bitmask& cpu_mask)
{
  // Launch thread.
  return std::thread([name, prio, cpu_mask, callable = std::move(callable)]() {
    pthread_t tself = pthread_self();
    if (pthread_setname_np(tself, name.c_str()) != 0) {
      perror("pthread_setname_np");
      fmt::print("Thread [{}]: Error while setting thread name to {}.\n", std::this_thread::get_id(), name);
    }

    // Set thread OS priority and affinity.
    // Note: TSAN seems to have issues with thread attributes when running as normal user, disable them in that case.
#ifndef HAVE_TSAN
    if (prio != os_thread_realtime_priority::NO_REALTIME) {
      thread_set_param(tself, prio);
    }
    if (cpu_mask.any()) {
      thread_set_affinity(tself, cpu_mask);
    }
#endif

    // Run task.
    callable();
  });
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
