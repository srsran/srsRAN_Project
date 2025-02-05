/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_crypto_token.h"

namespace srsran {

pdcp_crypto_token::pdcp_crypto_token(pdcp_crypto_token_manager& mngr_) : mngr(mngr_)
{
  mngr.increment_token();
}

pdcp_crypto_token::~pdcp_crypto_token()
{
  if (not was_moved) {
    mngr.return_token();
  }
}

} // namespace srsran
