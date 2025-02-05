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
  explicit pdcp_crypto_token_manager(timer_factory ue_timer_factory_, const pdcp_bearer_logger& logger_) :
    ue_timer_factory(ue_timer_factory_), logger(logger_)
  {
  }

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
    tokens++;
    logger.log_debug("Increased token count. tokens={}", tokens);
  }

  void return_token()
  {
    srsran_assert(tokens != 0, "Error counting crypto tokens. There are less tokens available then the ones granted.");
    tokens--;
    if (stopped) {
      set_once();
    }
    logger.log_debug("Decreased token count. tokens={}", tokens);
  }

  void set_once()
  {
    if (not was_set) {
      logger.log_debug("Set manual event flag");
      pending_crypto.set();
    }
    was_set = true;
  }

  bool              stopped = false;
  bool              was_set = false;
  manual_event_flag pending_crypto;

  uint32_t                  tokens = 0;
  timer_factory             ue_timer_factory;
  unique_timer              retry_timer;
  const pdcp_bearer_logger& logger;

  friend class pdcp_crypto_token;
};

} // namespace srsran
