/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_FAPI_VALIDATORS_UCI_HELPER_H
#define SRSGNB_UNITTESTS_FAPI_VALIDATORS_UCI_HELPER_H

#include "srsgnb/fapi/messages.h"
#include <random>

namespace unittest {

srsgnb::fapi::uci_pusch_pdu build_valid_uci_push_pdu();

srsgnb::fapi::uci_pucch_pdu_format_0_1 build_valid_uci_pucch_format01_pdu();

srsgnb::fapi::uci_pucch_pdu_format_2_3_4 build_valid_uci_pucch_format234_pdu();

srsgnb::fapi::uci_indication_message build_valid_uci_indication();

} // namespace unittest

#endif // SRSGNB_UNITTESTS_FAPI_VALIDATORS_UCI_HELPER_H
