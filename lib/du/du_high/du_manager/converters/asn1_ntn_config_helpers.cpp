/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "asn1_ntn_config_helpers.h"

using namespace asn1::rrc_nr;

sib19_r17_s srsran::srs_du::make_asn1_rrc_cell_sib19(const sib19_info& sib19_params)
{
  sib19_r17_s sib19;

  // Distance Threshold.
  sib19.distance_thresh_r17_present = false;

  // T-Service, currently not supported.
  sib19.t_service_r17_present = false;

  // NTN-Config
  sib19.ntn_cfg_r17_present = true;

  // Currently not supported fields.
  sib19.ntn_cfg_r17.kmac_r17_present                = false;
  sib19.ntn_cfg_r17.ntn_polarization_dl_r17_present = false;
  sib19.ntn_cfg_r17.ntn_polarization_ul_r17_present = false;
  sib19.ntn_cfg_r17.ta_report_r17_present           = false;

  // Cell-specific-k-offset.
  if (sib19_params.cell_specific_koffset.has_value()) {
    sib19.ntn_cfg_r17.cell_specific_koffset_r17_present = true;
    sib19.ntn_cfg_r17.cell_specific_koffset_r17         = sib19_params.cell_specific_koffset.value();
  }

  // The remaining parameters can be updated without system information change notifications nor a modification of
  // valueTag in SIB1.

  // Epoch Time.
  if (sib19_params.epoch_time.has_value()) {
    sib19.ntn_cfg_r17.epoch_time_r17_present          = true;
    sib19.ntn_cfg_r17.epoch_time_r17.sfn_r17          = sib19_params.epoch_time.value().sfn;
    sib19.ntn_cfg_r17.epoch_time_r17.sub_frame_nr_r17 = sib19_params.epoch_time.value().subframe_number;
  }

  // TA-Info.
  if (sib19_params.ta_info.has_value()) {
    sib19.ntn_cfg_r17.ta_info_r17_present       = true;
    ta_info_r17_s& ta_info                      = sib19.ntn_cfg_r17.ta_info_r17;
    ta_info.ta_common_drift_r17_present         = true;
    ta_info.ta_common_drift_variant_r17_present = true;
    ta_info.ta_common_r17                       = static_cast<uint32_t>(sib19_params.ta_info->ta_common / 0.004072);
    ta_info.ta_common_drift_r17                 = static_cast<int32_t>(sib19_params.ta_info->ta_common_drift / 0.0002);
    ta_info.ta_common_drift_variant_r17 =
        static_cast<uint16_t>(sib19_params.ta_info->ta_common_drift_variant / 0.00002);
    ta_info.ta_common_r17 += static_cast<int32_t>(sib19_params.ta_info->ta_common_offset / 0.004072);
  }

  // Ephemeris info.
  if (sib19_params.ephemeris_info.has_value()) {
    sib19.ntn_cfg_r17.ephemeris_info_r17_present = true;
    if (const auto* pos_vel = std::get_if<ecef_coordinates_t>(&sib19_params.ephemeris_info.value())) {
      sib19.ntn_cfg_r17.ephemeris_info_r17.set_position_velocity_r17();
      position_velocity_r17_s& rv = sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17();
      rv.position_x_r17           = static_cast<int32_t>(pos_vel->position_x / 1.3);
      rv.position_y_r17           = static_cast<int32_t>(pos_vel->position_y / 1.3);
      rv.position_z_r17           = static_cast<int32_t>(pos_vel->position_z / 1.3);
      rv.velocity_vx_r17          = static_cast<int32_t>(pos_vel->velocity_vx / 0.06);
      rv.velocity_vy_r17          = static_cast<int32_t>(pos_vel->velocity_vy / 0.06);
      rv.velocity_vz_r17          = static_cast<int32_t>(pos_vel->velocity_vz / 0.06);
    } else {
      const auto& orbital_elem = std::get<orbital_coordinates_t>(sib19_params.ephemeris_info.value());
      sib19.ntn_cfg_r17.ephemeris_info_r17.set_orbital_r17();
      orbital_r17_s& orbit      = sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17();
      orbit.semi_major_axis_r17 = static_cast<uint64_t>((orbital_elem.semi_major_axis - 6500000) / 0.004249);
      orbit.eccentricity_r17    = static_cast<uint32_t>(orbital_elem.eccentricity / 0.00000001431);
      orbit.inclination_r17     = static_cast<int32_t>(orbital_elem.inclination / 0.00000002341);
      orbit.longitude_r17       = static_cast<uint32_t>(orbital_elem.longitude / 0.00000002341);
      orbit.periapsis_r17       = static_cast<uint32_t>(orbital_elem.periapsis / 0.00000002341);
      orbit.mean_anomaly_r17    = static_cast<uint32_t>(orbital_elem.mean_anomaly / 0.00000002341);
    }
  }

  if (sib19_params.ntn_ul_sync_validity_dur.has_value()) {
    sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17_present = true;
    switch (sib19_params.ntn_ul_sync_validity_dur.value()) {
      case 5:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s5;
        break;
      case 10:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s10;
        break;
      case 15:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s15;
        break;
      case 20:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s20;
        break;
      case 25:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s25;
        break;
      case 30:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s30;
        break;
      case 35:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s35;
        break;
      case 40:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s40;
        break;
      case 45:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s45;
        break;
      case 50:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s50;
        break;
      case 55:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s55;
        break;
      case 60:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s60;
        break;
      case 120:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s120;
        break;
      case 180:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s180;
        break;
      case 240:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s240;
        break;
      case 900:
        sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17.value = ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::s900;
        break;
      default:
        report_fatal_error("Invalid ntn_ul_sync_validity_dur {}.", sib19_params.ntn_ul_sync_validity_dur.value());
    }
  }

  make_asn1_rrc_advanced_cell_sib19(sib19_params, sib19);

  return sib19;
}

#ifndef SRSRAN_HAS_ENTERPRISE_NTN

void srsran::srs_du::make_asn1_rrc_advanced_cell_sib19(const sib19_info& sib19_params, sib19_r17_s& out)
{
  // Encoding of the advanced NTN config parameters are not implemented.
}

#endif // SRSRAN_HAS_ENTERPRISE_NTN
