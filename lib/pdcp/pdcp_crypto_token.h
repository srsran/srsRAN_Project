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

#include "pdcp_bearer_logger.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/timers.h"

namespace srsran {

class pdcp_crypto_token_manager;

class pdcp_crypto_token
{
public:
  pdcp_crypto_token(pdcp_crypto_token_manager& mngr_);

  pdcp_crypto_token(pdcp_crypto_token&& obj) noexcept : mngr(obj.mngr) { obj.was_moved = true; }
  pdcp_crypto_token& operator=(pdcp_crypto_token&&)      = delete;
  pdcp_crypto_token(const pdcp_crypto_token&)            = delete;
  pdcp_crypto_token& operator=(const pdcp_crypto_token&) = delete;

  ~pdcp_crypto_token();

private:
  bool                       was_moved = false;
  pdcp_crypto_token_manager& mngr;
};

class pdcp_crypto_token_manager
{
public:
  explicit pdcp_crypto_token_manager() = default;

  manual_event_flag& get_awaitable() { return pending_crypto; }

  void stop()
  {
    stopped = true;
    if (tokens == 0) {
      set_once();
    }
  }

  [[nodiscard]] pdcp_crypto_token get_token() { return pdcp_crypto_token{*this}; }

private:
  void increment_token()
  {
    srsran_assert(not stopped, "Trying to allocate more tokens, even though token manager has stopped.");
    tokens.fetch_add(1, std::memory_order_relaxed);
  }

  void return_token()
  {
    uint32_t prev_token = tokens.fetch_sub(1, std::memory_order_relaxed);
    srsran_assert(prev_token != 0,
                  "Error counting crypto tokens. There are less tokens available then the ones granted. token={}",
                  prev_token);
    if (stopped and prev_token == 1) {
      set_once();
    }
  }

  void set_once()
  {
    if (not was_set) {
      pending_crypto.set();
      was_set = true;
    }
  }

  bool              stopped = false;
  bool              was_set = false;
  manual_event_flag pending_crypto;

  std::atomic<uint32_t> tokens = 0;

  friend class pdcp_crypto_token;
};

} // namespace srsran
