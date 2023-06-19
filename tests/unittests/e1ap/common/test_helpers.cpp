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
#include "srsran/support/test_utils.h"

using namespace srsran;

gnb_cu_cp_ue_e1ap_id_t srsran::generate_random_gnb_cu_cp_ue_e1ap_id()
{
  return int_to_gnb_cu_cp_ue_e1ap_id(
      test_rgen::uniform_int<uint64_t>(gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t::min),
                                       gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t::max) - 1));
}

gnb_cu_up_ue_e1ap_id_t srsran::generate_random_gnb_cu_up_ue_e1ap_id()
{
  return int_to_gnb_cu_up_ue_e1ap_id(
      test_rgen::uniform_int<uint64_t>(gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::min),
                                       gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::max) - 1));
}
