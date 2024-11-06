/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
