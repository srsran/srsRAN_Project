/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/support/async/manual_event.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class pdcp_crypto_token_manager
{
public:
  explicit pdcp_crypto_token_manager() = default;

  manual_event_flag& get_awaitable() { return pending_crypto; }

  void start()
  {
    was_set = false;
    pending_crypto.reset();
    increment_token();
  }

  void stop() { return_token(); }

private:
  void increment_token() { tokens.fetch_add(1, std::memory_order_relaxed); }

  void return_token()
  {
    uint32_t prev_token = tokens.fetch_sub(1, std::memory_order_relaxed);
    srsran_assert(prev_token != UINT32_MAX,
                  "Error counting crypto tokens. There are less tokens available then the ones granted.");

    // If there is one less token then the ones granted, the stop() has been called,
    // and all tokens have been returned.
    if (prev_token == 0) {
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

  bool              was_set = false;
  manual_event_flag pending_crypto;

  std::atomic<uint32_t> tokens = 0;

  friend class pdcp_crypto_token;
};

class pdcp_crypto_token
{
public:
  pdcp_crypto_token(pdcp_crypto_token_manager& mngr_) : mngr(mngr_) { mngr.increment_token(); }

  ~pdcp_crypto_token()
  {
    if (not was_moved) {
      mngr.return_token();
    }
  }

  pdcp_crypto_token(pdcp_crypto_token&& obj) noexcept : mngr(obj.mngr) { obj.was_moved = true; }
  pdcp_crypto_token& operator=(pdcp_crypto_token&&)      = delete;
  pdcp_crypto_token(const pdcp_crypto_token&)            = delete;
  pdcp_crypto_token& operator=(const pdcp_crypto_token&) = delete;

private:
  bool                       was_moved = false;
  pdcp_crypto_token_manager& mngr;
};

} // namespace srsran
