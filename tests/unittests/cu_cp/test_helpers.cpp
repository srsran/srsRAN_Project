/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "test_helpers.h"

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

gnb_cu_cp_ue_e1ap_id_t srsran::srs_cu_cp::generate_random_gnb_cu_cp_ue_e1ap_id()
{
  return int_to_gnb_cu_cp_ue_e1ap_id(
      test_rgen::uniform_int<uint64_t>(gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t::min),
                                       gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t::max) - 1));
}

gnb_cu_up_ue_e1ap_id_t srsran::srs_cu_cp::generate_random_gnb_cu_up_ue_e1ap_id()
{
  return int_to_gnb_cu_up_ue_e1ap_id(
      test_rgen::uniform_int<uint64_t>(gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::min),
                                       gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::max) - 1));
}

gnb_cu_ue_f1ap_id_t srsran::srs_cu_cp::generate_random_gnb_cu_ue_f1ap_id()
{
  return int_to_gnb_cu_ue_f1ap_id(test_rgen::uniform_int<uint64_t>(
      gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id_t::min), gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id_t::max) - 1));
}

gnb_du_ue_f1ap_id_t srsran::srs_cu_cp::generate_random_gnb_du_ue_f1ap_id()
{
  return int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint64_t>(
      gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id_t::min), gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id_t::max) - 1));
}
