/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_packed_test_messages.h"
#include "srsran/asn1/rrc_nr/ho_prep_info.h"

using namespace srsran;

byte_buffer srsran::create_ho_prep_info()
{
  ho_prep_info_s ho_prep;
  ho_prep.crit_exts.set_c1().set_ho_prep_info();

  // pack.
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (ho_prep.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}
