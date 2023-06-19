/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "test_helpers.h"
#include "../rrc/rrc_ue_test_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

byte_buffer srsran::srs_cu_cp::generate_container_with_cell_group_config()
{
  asn1::unbounded_octstring<true> octet_str;
  octet_str.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");
  return octet_str.to_byte_buffer();
}

byte_buffer srsran::srs_cu_cp::generate_rrc_setup_complete()
{
  asn1::unbounded_octstring<true> octet_str;
  octet_str.from_string(
      "000010c01000082727e01c3ff100c047e004139000bf202f8998000410000000f2e04f070f0707100517e004139000bf202f899800041000"
      "0000f1001032e04f070f0702f1b08010027db00000000080101b669000000000801000001000000005202f8990000011707f070c0401980b"
      "018010174000090530101000000000");
  return octet_str.to_byte_buffer();
}
