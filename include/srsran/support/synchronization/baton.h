/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Baton synchronization primitive.
///
/// It is used to synchronize two threads, where one waits for the other to finish.
class baton
{
public:
  void post()
  {
    std::lock_guard<std::mutex> lock(mutex);
    is_set = true;
    cvar.notify_one();
  }

  void wait()
  {
    std::unique_lock<std::mutex> lock(mutex);
    cvar.wait(lock, [this] { return is_set; });
  }

private:
  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    is_set = false;
};

/// RAII helper for baton synchronization primitive.
class scoped_baton_sender
{
  struct deleter {
    void operator()(baton* p)
    {
      if (p != nullptr) {
        p->post();
      }
    }
  };

public:
  scoped_baton_sender(baton& parent_) : parent(&parent_) {}
  void post() { parent = nullptr; }

private:
  // Use of unique_ptr for RAII
  std::unique_ptr<baton, deleter> parent;
};

} // namespace srsran
