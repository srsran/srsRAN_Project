/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "asn1_csi_meas_config_helpers.h"
#include "../../asn1/asn1_diff_utils.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;
using namespace asn1::rrc_nr;

asn1::rrc_nr::nzp_csi_rs_res_set_s make_asn1_nzp_csi_rs_resource(const nzp_csi_rs_resource& cfg)
{
  nzp_csi_rs_res_set_s out{};

  return out;
}

void srsgnb::srs_du::calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
                                                    const csi_meas_config&        src,
                                                    const csi_meas_config&        dest)
{
  calculate_addmodremlist_diff(
      out.nzp_csi_rs_res_set_to_add_mod_list,
      out.nzp_csi_rs_res_set_to_release_list,
      src.nzp_csi_rs_res_list,
      dest.nzp_csi_rs_res_list,
      [](const nzp_csi_rs_resource& res) { return make_asn1_nzp_csi_rs_resource(res); },
      [](const nzp_csi_rs_resource& res) { return res.res_id; });
}
