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
#include "srsran/support/timers.h"

namespace srsran {

class pdcp_crypto_token_manager;

class pdcp_crypto_token
{
public:
  pdcp_crypto_token(pdcp_crypto_token_manager& mngr_);

  pdcp_crypto_token(pdcp_crypto_token&&)                 = default;
  pdcp_crypto_token& operator=(pdcp_crypto_token&&)      = delete;
  pdcp_crypto_token(const pdcp_crypto_token&)            = delete;
  pdcp_crypto_token& operator=(const pdcp_crypto_token&) = delete;

  ~pdcp_crypto_token();

private:
  pdcp_crypto_token_manager& mngr;
};

class pdcp_crypto_token_manager
{
public:
  explicit pdcp_crypto_token_manager(timer_factory ue_timer_factory_, const pdcp_bearer_logger& logger_) :
    ue_timer_factory(ue_timer_factory_), logger(logger_)
  {
    retry_timer = ue_timer_factory.create_timer();
    retry_timer.set(std::chrono::milliseconds(5), [this](timer_id_t tid) { set_flag_when_finished(); });
  }

  manual_event_flag& get_awaitable() { return pending_crypto; }

  void set_flag_when_finished()
  {
    if (tokens == 0) {
      set_once();
    } else {
      // Retry running the timer.
      retry_timer.run();
    }
  }

  [[nodiscard]] pdcp_crypto_token get_token() { return pdcp_crypto_token{*this}; }

private:
  void increment_token()
  {
    tokens++;
    logger.log_debug("Increased token count. tokens={}", tokens);
  }

  void return_token()
  {
    tokens--;
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

  bool              was_set = false;
  manual_event_flag pending_crypto;

  uint32_t                  tokens = 0;
  timer_factory             ue_timer_factory;
  unique_timer              retry_timer;
  const pdcp_bearer_logger& logger;

  friend class pdcp_crypto_token;
};

} // namespace srsran
