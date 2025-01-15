/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
