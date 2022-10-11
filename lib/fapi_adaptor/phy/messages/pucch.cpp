/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/pucch.h"

using namespace srsgnb;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_pucch_fapi_to_phy(uplink_processor::pucch_pdu& pdu,
                                                     const fapi::ul_pucch_pdu&    fapi_pdu,
                                                     uint16_t                     sfn,
                                                     uint16_t                     slot)
{
}
