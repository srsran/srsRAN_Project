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

#include "rrc_measurement_types_asn1_converters.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/error_handling.h"
#include <type_traits>

using namespace srsran;
using namespace srs_cu_cp;

rrc_ssb_mtc srsran::srs_cu_cp::asn1_to_ssb_mtc(const asn1::rrc_nr::ssb_mtc_s& asn1_ssb_mtc)
{
  rrc_ssb_mtc ssb_mtc;

  // periodicity and offset
  switch (asn1_ssb_mtc.periodicity_and_offset.type()) {
    case asn1::rrc_nr::ssb_mtc_s::periodicity_and_offset_c_::types_opts::options::sf5:
      ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
      ssb_mtc.periodicity_and_offset.offset      = asn1_ssb_mtc.periodicity_and_offset.sf5();
      break;
    case asn1::rrc_nr::ssb_mtc_s::periodicity_and_offset_c_::types_opts::options::sf10:
      ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf10;
      ssb_mtc.periodicity_and_offset.offset      = asn1_ssb_mtc.periodicity_and_offset.sf10();
      break;
    case asn1::rrc_nr::ssb_mtc_s::periodicity_and_offset_c_::types_opts::options::sf20:
      ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf20;
      ssb_mtc.periodicity_and_offset.offset      = asn1_ssb_mtc.periodicity_and_offset.sf20();
      break;
    case asn1::rrc_nr::ssb_mtc_s::periodicity_and_offset_c_::types_opts::options::sf40:
      ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf40;
      ssb_mtc.periodicity_and_offset.offset      = asn1_ssb_mtc.periodicity_and_offset.sf40();
      break;
    case asn1::rrc_nr::ssb_mtc_s::periodicity_and_offset_c_::types_opts::options::sf80:
      ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf80;
      ssb_mtc.periodicity_and_offset.offset      = asn1_ssb_mtc.periodicity_and_offset.sf80();
      break;
    case asn1::rrc_nr::ssb_mtc_s::periodicity_and_offset_c_::types_opts::options::sf160:
      ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf160;
      ssb_mtc.periodicity_and_offset.offset      = asn1_ssb_mtc.periodicity_and_offset.sf160();
      break;
    default:
      srslog::fetch_basic_logger("RRC").error("Invalid SSB MTC configuration.");
  }

  // dur
  ssb_mtc.dur = asn1_ssb_mtc.dur.to_number();

  return ssb_mtc;
}

subcarrier_spacing
srsran::srs_cu_cp::rrc_asn1_to_subcarrier_spacing(const asn1::rrc_nr::subcarrier_spacing_e asn1_sc_spacing)
{
  subcarrier_spacing sc_spacing;

  switch (asn1_sc_spacing) {
    case asn1::rrc_nr::subcarrier_spacing_opts::options::khz15:
      sc_spacing = srsran::subcarrier_spacing::kHz15;
      break;
    case asn1::rrc_nr::subcarrier_spacing_opts::options::khz30:
      sc_spacing = srsran::subcarrier_spacing::kHz30;
      break;
    case asn1::rrc_nr::subcarrier_spacing_opts::options::khz60:
      sc_spacing = srsran::subcarrier_spacing::kHz60;
      break;
    case asn1::rrc_nr::subcarrier_spacing_opts::options::khz120:
      sc_spacing = srsran::subcarrier_spacing::kHz120;
      break;
    case asn1::rrc_nr::subcarrier_spacing_opts::options::khz240:
      sc_spacing = srsran::subcarrier_spacing::kHz240;
      break;
    default:
      sc_spacing = srsran::subcarrier_spacing::invalid;
  }

  return sc_spacing;
}

rrc_meas_timing srsran::srs_cu_cp::asn1_to_meas_timing(const asn1::rrc_nr::meas_timing_s& asn1_meas_timing)
{
  rrc_meas_timing meas_timing;

  // freq and timing
  if (asn1_meas_timing.freq_and_timing_present) {
    rrc_meas_timing::rrc_freq_and_timing_ freq_and_timing;

    // carrier freq
    freq_and_timing.carrier_freq = asn1_meas_timing.freq_and_timing.carrier_freq;

    // subcarrier spacing
    freq_and_timing.ssb_subcarrier_spacing =
        rrc_asn1_to_subcarrier_spacing(asn1_meas_timing.freq_and_timing.ssb_subcarrier_spacing);

    // ssb mtc
    freq_and_timing.ssb_meas_timing_cfg = asn1_to_ssb_mtc(asn1_meas_timing.freq_and_timing.ssb_meas_timing_cfg);

    // ss rssi meas
    if (asn1_meas_timing.freq_and_timing.ss_rssi_meas_present) {
      rrc_ss_rssi_meas ss_rssi_meas;

      // meas slots
      ss_rssi_meas.meas_slots = asn1_meas_timing.freq_and_timing.ss_rssi_meas.meas_slots.to_number();
      // end symbol
      ss_rssi_meas.end_symbol = asn1_meas_timing.freq_and_timing.ss_rssi_meas.end_symbol;

      freq_and_timing.ss_rssi_meas = ss_rssi_meas;
    }

    meas_timing.freq_and_timing = freq_and_timing;
  }

  return meas_timing;
}

asn1::rrc_nr::subcarrier_spacing_e srsran::srs_cu_cp::subcarrier_spacing_to_rrc_asn1(subcarrier_spacing sc_spacing)
{
  asn1::rrc_nr::subcarrier_spacing_e asn1_sc_spacing;

  switch (sc_spacing) {
    case srsran::subcarrier_spacing::kHz15:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz15;
      break;
    case srsran::subcarrier_spacing::kHz30:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
      break;
    case srsran::subcarrier_spacing::kHz60:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz60;
      break;
    case srsran::subcarrier_spacing::kHz120:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz120;
      break;
    case srsran::subcarrier_spacing::kHz240:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz240;
      break;
    default:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::nulltype;
  }

  return asn1_sc_spacing;
}

asn1::rrc_nr::ssb_mtc_s srsran::srs_cu_cp::ssb_mtc_to_rrc_asn1(rrc_ssb_mtc ssb_mtc)
{
  asn1::rrc_nr::ssb_mtc_s asn1_ssb_mtc;

  switch (ssb_mtc.periodicity_and_offset.periodicity) {
    case rrc_periodicity_and_offset::periodicity_t::sf5:
      asn1_ssb_mtc.periodicity_and_offset.set_sf5() = ssb_mtc.periodicity_and_offset.offset;
      break;
    case rrc_periodicity_and_offset::periodicity_t::sf10:
      asn1_ssb_mtc.periodicity_and_offset.set_sf10() = ssb_mtc.periodicity_and_offset.offset;
      break;
    case rrc_periodicity_and_offset::periodicity_t::sf20:
      asn1_ssb_mtc.periodicity_and_offset.set_sf20() = ssb_mtc.periodicity_and_offset.offset;
      break;
    case rrc_periodicity_and_offset::periodicity_t::sf40:
      asn1_ssb_mtc.periodicity_and_offset.set_sf40() = ssb_mtc.periodicity_and_offset.offset;
      break;
    case rrc_periodicity_and_offset::periodicity_t::sf80:
      asn1_ssb_mtc.periodicity_and_offset.set_sf80() = ssb_mtc.periodicity_and_offset.offset;
      break;
    case rrc_periodicity_and_offset::periodicity_t::sf160:
      asn1_ssb_mtc.periodicity_and_offset.set_sf160() = ssb_mtc.periodicity_and_offset.offset;
      break;
    default:
      report_fatal_error("Cannot convert SSB MTC to ASN.1 type");
  }

  asn1::number_to_enum(asn1_ssb_mtc.dur, ssb_mtc.dur);

  return asn1_ssb_mtc;
}

asn1::rrc_nr::ssb_cfg_mob_s srsran::srs_cu_cp::ssb_cfg_mob_to_rrc_asn1(const rrc_ssb_cfg_mob& ssb_cfg_mob)
{
  asn1::rrc_nr::ssb_cfg_mob_s asn1_ssb_cfg_mob;

  // ssb to measure
  if (ssb_cfg_mob.ssb_to_measure.has_value()) {
    const auto& ssb_to_measure              = ssb_cfg_mob.ssb_to_measure.value();
    asn1_ssb_cfg_mob.ssb_to_measure_present = true;
    // release
    if (ssb_to_measure.is_release) {
      asn1_ssb_cfg_mob.ssb_to_measure.set_release();
    } else if (ssb_to_measure.setup.has_value()) {
      // setup
      asn1_ssb_cfg_mob.ssb_to_measure.set_setup();
      // short bitmap
      if (ssb_to_measure.setup.value().type == rrc_ssb_to_measure::bitmap_type_t::short_bitmap) {
        asn1_ssb_cfg_mob.ssb_to_measure.setup().set_short_bitmap();
        asn1_ssb_cfg_mob.ssb_to_measure.setup().short_bitmap().from_number(
            static_cast<uint8_t>(ssb_to_measure.setup.value().bitmap));
      }
      // medium bitmap
      if (ssb_to_measure.setup.value().type == rrc_ssb_to_measure::bitmap_type_t::medium_bitmap) {
        asn1_ssb_cfg_mob.ssb_to_measure.setup().set_medium_bitmap();
        asn1_ssb_cfg_mob.ssb_to_measure.setup().medium_bitmap().from_number(
            static_cast<uint8_t>(ssb_to_measure.setup.value().bitmap));
      }
      // long bitmap
      if (ssb_to_measure.setup.value().type == rrc_ssb_to_measure::bitmap_type_t::long_bitmap) {
        asn1_ssb_cfg_mob.ssb_to_measure.setup().set_long_bitmap();
        asn1_ssb_cfg_mob.ssb_to_measure.setup().long_bitmap().from_number(ssb_to_measure.setup.value().bitmap);
      }
    }
  }

  // derive ssb idx from cell
  asn1_ssb_cfg_mob.derive_ssb_idx_from_cell = ssb_cfg_mob.derive_ssb_idx_from_cell;

  // ss rssi meas
  if (ssb_cfg_mob.ss_rssi_meas.has_value()) {
    const auto& ss_rssi_meas              = ssb_cfg_mob.ss_rssi_meas.value();
    asn1_ssb_cfg_mob.ss_rssi_meas_present = true;
    asn1_ssb_cfg_mob.ss_rssi_meas.meas_slots.from_number(ss_rssi_meas.meas_slots);
    asn1_ssb_cfg_mob.ss_rssi_meas.end_symbol = ss_rssi_meas.end_symbol;
  }

  return asn1_ssb_cfg_mob;
}

asn1::setup_release_c<asn1::rrc_nr::csi_rs_res_cfg_mob_s>
srsran::srs_cu_cp::csi_res_cfg_mob_to_rrc_asn1(const rrc_csi_rs_res_cfg_mob_setup_release& csi_rs_res_cfg_mob)
{
  asn1::setup_release_c<asn1::rrc_nr::csi_rs_res_cfg_mob_s> asn1_csi_rs_res_cfg_mob;

  if (csi_rs_res_cfg_mob.is_release) {
    asn1_csi_rs_res_cfg_mob.set_release();
  } else if (csi_rs_res_cfg_mob.setup.has_value()) {
    asn1_csi_rs_res_cfg_mob.set_setup();
    // subcarrier spacing
    asn1_csi_rs_res_cfg_mob.setup().subcarrier_spacing =
        subcarrier_spacing_to_rrc_asn1(csi_rs_res_cfg_mob.setup.value().sc_spacing);
    // csi rs cell list mob
    for (const auto& csi_rs_cell_mob : csi_rs_res_cfg_mob.setup.value().csi_rs_cell_list_mob) {
      asn1::rrc_nr::csi_rs_cell_mob_s asn1_csi_rs_cell_mob;

      // cell id
      asn1_csi_rs_cell_mob.cell_id = csi_rs_cell_mob.cell_id;
      // csi rs meas bw
      asn1::number_to_enum(asn1_csi_rs_cell_mob.csi_rs_meas_bw.nrof_prbs, csi_rs_cell_mob.csi_rs_meas_bw.nrof_prbs);
      asn1_csi_rs_cell_mob.csi_rs_meas_bw.start_prb = csi_rs_cell_mob.csi_rs_meas_bw.start_prb;
      // density
      if (csi_rs_cell_mob.density.has_value()) {
        asn1_csi_rs_cell_mob.density_present = true;
        asn1::number_to_enum(asn1_csi_rs_cell_mob.density, csi_rs_cell_mob.density.value());
      }
      // csi rs res list mob
      for (const auto& csi_rs_res_mob : csi_rs_cell_mob.csi_rs_res_list_mob) {
        asn1::rrc_nr::csi_rs_res_mob_s asn1_csi_rs_res_mob;

        // csi rs idx
        asn1_csi_rs_res_mob.csi_rs_idx = csi_rs_res_mob.csi_rs_idx;
        // slot cfg
        if (csi_rs_res_mob.slot_cfg.period == rrc_slot_cfg::period_t::ms4) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms4() = static_cast<uint8_t>(csi_rs_res_mob.slot_cfg.offset);
        }
        if (csi_rs_res_mob.slot_cfg.period == rrc_slot_cfg::period_t::ms5) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms5() = static_cast<uint8_t>(csi_rs_res_mob.slot_cfg.offset);
        }
        if (csi_rs_res_mob.slot_cfg.period == rrc_slot_cfg::period_t::ms10) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms10() = static_cast<uint8_t>(csi_rs_res_mob.slot_cfg.offset);
        }
        if (csi_rs_res_mob.slot_cfg.period == rrc_slot_cfg::period_t::ms20) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms20() = static_cast<uint8_t>(csi_rs_res_mob.slot_cfg.offset);
        }
        if (csi_rs_res_mob.slot_cfg.period == rrc_slot_cfg::period_t::ms40) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms40() = csi_rs_res_mob.slot_cfg.offset;
        }
        // associated ssb
        if (csi_rs_res_mob.associated_ssb.has_value()) {
          asn1_csi_rs_res_mob.associated_ssb_present = true;
          asn1_csi_rs_res_mob.associated_ssb.ssb_idx = csi_rs_res_mob.associated_ssb.value().ssb_idx;
          asn1_csi_rs_res_mob.associated_ssb.is_quasi_colocated =
              csi_rs_res_mob.associated_ssb.value().is_quasi_colocated;
        }
        // freq domain alloc
        if (std::holds_alternative<rrc_freq_domain_alloc_row1>(csi_rs_res_mob.freq_domain_alloc)) {
          asn1_csi_rs_res_mob.freq_domain_alloc.set_row1();
          asn1_csi_rs_res_mob.freq_domain_alloc.row1().from_number(
              std::get<rrc_freq_domain_alloc_row1>(csi_rs_res_mob.freq_domain_alloc));
        }
        if (std::holds_alternative<rrc_freq_domain_alloc_row2>(csi_rs_res_mob.freq_domain_alloc)) {
          asn1_csi_rs_res_mob.freq_domain_alloc.set_row2();
          asn1_csi_rs_res_mob.freq_domain_alloc.row2().from_number(
              std::get<rrc_freq_domain_alloc_row2>(csi_rs_res_mob.freq_domain_alloc));
        }
        // first ofdm symbol in time domain
        asn1_csi_rs_res_mob.first_ofdm_symbol_in_time_domain = csi_rs_res_mob.first_ofdm_symbol_in_time_domain;
        // seq generation cfg
        asn1_csi_rs_res_mob.seq_generation_cfg = csi_rs_res_mob.seq_generation_cfg;

        asn1_csi_rs_cell_mob.csi_rs_res_list_mob.push_back(asn1_csi_rs_res_mob);
      }

      asn1_csi_rs_res_cfg_mob.setup().csi_rs_cell_list_mob.push_back(asn1_csi_rs_cell_mob);
    }
  } else {
    // error
    report_fatal_error("Cannot convert CSI RS res cfg mob to ASN.1 type");
  }

  return asn1_csi_rs_res_cfg_mob;
}

asn1::rrc_nr::thres_nr_s srsran::srs_cu_cp::thres_nr_to_rrc_asn1(const rrc_thres_nr& thres_nr)
{
  asn1::rrc_nr::thres_nr_s asn1_thres_nr;

  // thres rsrp
  if (thres_nr.thres_rsrp.has_value()) {
    asn1_thres_nr.thres_rsrp_present = true;
    asn1_thres_nr.thres_rsrp         = thres_nr.thres_rsrp.value();
  }

  // thres rsrq
  if (thres_nr.thres_rsrq.has_value()) {
    asn1_thres_nr.thres_rsrq_present = true;
    asn1_thres_nr.thres_rsrq         = thres_nr.thres_rsrq.value();
  }

  // thres sinr
  if (thres_nr.thres_sinr.has_value()) {
    asn1_thres_nr.thres_sinr_present = true;
    asn1_thres_nr.thres_sinr         = thres_nr.thres_sinr.value();
  }

  return asn1_thres_nr;
}

asn1::rrc_nr::q_offset_range_list_s
srsran::srs_cu_cp::q_offset_range_list_to_rrc_asn1(const rrc_q_offset_range_list& q_offset_range_list)
{
  asn1::rrc_nr::q_offset_range_list_s asn1_q_offset_range_list;

  // rsrp offset ssb
  if (q_offset_range_list.rsrp_offset_ssb.has_value()) {
    asn1_q_offset_range_list.rsrp_offset_ssb_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrp_offset_ssb, q_offset_range_list.rsrp_offset_ssb.value());
  }
  // rsrq offset ssb
  if (q_offset_range_list.rsrq_offset_ssb.has_value()) {
    asn1_q_offset_range_list.rsrq_offset_ssb_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrq_offset_ssb, q_offset_range_list.rsrq_offset_ssb.value());
  }
  // sinr offset ssb
  if (q_offset_range_list.sinr_offset_ssb.has_value()) {
    asn1_q_offset_range_list.sinr_offset_ssb_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.sinr_offset_ssb, q_offset_range_list.sinr_offset_ssb.value());
  }
  // rsrp offset csi_rs
  if (q_offset_range_list.rsrp_offset_csi_rs.has_value()) {
    asn1_q_offset_range_list.rsrp_offset_csi_rs_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrp_offset_csi_rs, q_offset_range_list.rsrp_offset_csi_rs.value());
  }
  // rsrq offset csi_rs
  if (q_offset_range_list.rsrq_offset_csi_rs.has_value()) {
    asn1_q_offset_range_list.rsrq_offset_csi_rs_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrq_offset_csi_rs, q_offset_range_list.rsrq_offset_csi_rs.value());
  }
  // sinr offset csi_rs
  if (q_offset_range_list.sinr_offset_csi_rs.has_value()) {
    asn1_q_offset_range_list.sinr_offset_csi_rs_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.sinr_offset_csi_rs, q_offset_range_list.sinr_offset_csi_rs.value());
  }

  return asn1_q_offset_range_list;
}

asn1::rrc_nr::meas_obj_nr_s srsran::srs_cu_cp::meas_obj_nr_to_rrc_asn1(const rrc_meas_obj_nr& meas_obj_nr)
{
  asn1::rrc_nr::meas_obj_nr_s asn1_meas_obj_nr;

  // ssb freq
  if (meas_obj_nr.ssb_freq.has_value()) {
    asn1_meas_obj_nr.ssb_freq_present = true;
    asn1_meas_obj_nr.ssb_freq         = meas_obj_nr.ssb_freq.value();
  }

  // ssb subcarrier spacing
  if (meas_obj_nr.ssb_subcarrier_spacing.has_value()) {
    asn1_meas_obj_nr.ssb_subcarrier_spacing_present = true;
    asn1_meas_obj_nr.ssb_subcarrier_spacing =
        subcarrier_spacing_to_rrc_asn1(meas_obj_nr.ssb_subcarrier_spacing.value());
  }

  // smtc1
  if (meas_obj_nr.smtc1.has_value()) {
    asn1_meas_obj_nr.smtc1_present = true;
    asn1_meas_obj_nr.smtc1         = ssb_mtc_to_rrc_asn1(meas_obj_nr.smtc1.value());
  }

  // smtc2
  if (meas_obj_nr.smtc2.has_value()) {
    asn1_meas_obj_nr.smtc2_present = true;
    // pci list
    for (const auto& pci : meas_obj_nr.smtc2.value().pci_list) {
      asn1_meas_obj_nr.smtc2.pci_list.push_back(pci);
    }
    // periodicity
    asn1::number_to_enum(asn1_meas_obj_nr.smtc2.periodicity, meas_obj_nr.smtc2.value().periodicity);
  }

  // ref freq csi rs
  if (meas_obj_nr.ref_freq_csi_rs.has_value()) {
    asn1_meas_obj_nr.ref_freq_csi_rs_present = true;
    asn1_meas_obj_nr.ref_freq_csi_rs         = meas_obj_nr.ref_freq_csi_rs.value();
  }

  // ref sig cfg
  // ssb cfg mob
  if (meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.has_value()) {
    asn1_meas_obj_nr.ref_sig_cfg.ssb_cfg_mob_present = true;
    asn1_meas_obj_nr.ref_sig_cfg.ssb_cfg_mob = ssb_cfg_mob_to_rrc_asn1(meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.value());
  }
  // csi rs res cfg mob
  if (meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob.has_value()) {
    asn1_meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob_present = true;
    asn1_meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob =
        csi_res_cfg_mob_to_rrc_asn1(meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob.value());
  }

  // abs thresh ss blocks consolidation
  if (meas_obj_nr.abs_thresh_ss_blocks_consolidation.has_value()) {
    asn1_meas_obj_nr.abs_thresh_ss_blocks_consolidation_present = true;
    asn1_meas_obj_nr.abs_thresh_ss_blocks_consolidation =
        thres_nr_to_rrc_asn1(meas_obj_nr.abs_thresh_ss_blocks_consolidation.value());
  }

  // abs thresh csi rs consolidation
  if (meas_obj_nr.abs_thresh_csi_rs_consolidation.has_value()) {
    asn1_meas_obj_nr.abs_thresh_csi_rs_consolidation_present = true;
    asn1_meas_obj_nr.abs_thresh_csi_rs_consolidation =
        thres_nr_to_rrc_asn1(meas_obj_nr.abs_thresh_csi_rs_consolidation.value());
  }

  // nrof ss blocks to average
  if (meas_obj_nr.nrof_ss_blocks_to_average.has_value()) {
    asn1_meas_obj_nr.nrof_ss_blocks_to_average_present = true;
    asn1_meas_obj_nr.nrof_ss_blocks_to_average         = meas_obj_nr.nrof_ss_blocks_to_average.value();
  }

  // nrof csi rs res to average
  if (meas_obj_nr.nrof_csi_rs_res_to_average.has_value()) {
    asn1_meas_obj_nr.nrof_csi_rs_res_to_average_present = true;
    asn1_meas_obj_nr.nrof_csi_rs_res_to_average         = meas_obj_nr.nrof_csi_rs_res_to_average.value();
  }

  // quant cfg idx
  asn1_meas_obj_nr.quant_cfg_idx = meas_obj_nr.quant_cfg_idx;

  // offset mo
  asn1_meas_obj_nr.offset_mo = q_offset_range_list_to_rrc_asn1(meas_obj_nr.offset_mo);

  // cells to rem list
  srsran_assert(meas_obj_nr.cells_to_rem_list.size() <= 32,
                "Too many cells to remove ({}>{}).",
                meas_obj_nr.cells_to_rem_list.size(),
                32);
  for (const auto& cell_to_rem : meas_obj_nr.cells_to_rem_list) {
    asn1_meas_obj_nr.cells_to_rem_list.push_back(cell_to_rem);
  }

  // cells to add mod list
  for (const auto& cell_to_add_mod : meas_obj_nr.cells_to_add_mod_list) {
    asn1::rrc_nr::cells_to_add_mod_s asn1_cells_to_add_mod;

    asn1_cells_to_add_mod.pci = cell_to_add_mod.pci;
    asn1_cells_to_add_mod.cell_individual_offset =
        q_offset_range_list_to_rrc_asn1(cell_to_add_mod.cell_individual_offset);

    asn1_meas_obj_nr.cells_to_add_mod_list.push_back(asn1_cells_to_add_mod);
  }

  // excluded cells to rem list
  srsran_assert(meas_obj_nr.excluded_cells_to_rem_list.size() <= 8,
                "Too many excluded cells to remove ({}>{}).",
                meas_obj_nr.excluded_cells_to_rem_list.size(),
                8);
  for (const auto& excluded_cell : meas_obj_nr.excluded_cells_to_rem_list) {
    asn1_meas_obj_nr.excluded_cells_to_rem_list.push_back(excluded_cell);
  }

  // excluded cells to add mod list
  for (const auto& excluded_cell : meas_obj_nr.excluded_cells_to_add_mod_list) {
    asn1::rrc_nr::pci_range_elem_s asn1_pci_range_elem;
    asn1_pci_range_elem.pci_range_idx = excluded_cell.pci_range_idx;

    asn1_pci_range_elem.pci_range.start = excluded_cell.pci_range.start;
    if (excluded_cell.pci_range.range.has_value()) {
      asn1_pci_range_elem.pci_range.range_present = true;
      asn1::number_to_enum(asn1_pci_range_elem.pci_range.range, excluded_cell.pci_range.range.value());
    }
    asn1_meas_obj_nr.excluded_cells_to_add_mod_list.push_back(asn1_pci_range_elem);
  }

  // allowed cells to rem list
  srsran_assert(meas_obj_nr.allowed_cells_to_rem_list.size() <= 8,
                "Too many allowed cells to remove ({}>{}).",
                meas_obj_nr.allowed_cells_to_rem_list.size(),
                8);
  for (const auto& allowed_cell : meas_obj_nr.allowed_cells_to_rem_list) {
    asn1_meas_obj_nr.allowed_cells_to_rem_list.push_back(allowed_cell);
  }

  // allowed cells to add mod list
  for (const auto& allowed_cell : meas_obj_nr.allowed_cells_to_add_mod_list) {
    asn1::rrc_nr::pci_range_elem_s asn1_pci_range_elem;
    asn1_pci_range_elem.pci_range_idx = allowed_cell.pci_range_idx;

    asn1_pci_range_elem.pci_range.start = allowed_cell.pci_range.start;
    if (allowed_cell.pci_range.range.has_value()) {
      asn1_pci_range_elem.pci_range.range_present = true;
      asn1::number_to_enum(asn1_pci_range_elem.pci_range.range, allowed_cell.pci_range.range.value());
    }
    asn1_meas_obj_nr.allowed_cells_to_add_mod_list.push_back(asn1_pci_range_elem);
  }

  // group 0
  // freq band ind nr
  if (meas_obj_nr.freq_band_ind_nr.has_value()) {
    asn1_meas_obj_nr.ext                      = true;
    asn1_meas_obj_nr.freq_band_ind_nr_present = true;
    asn1_meas_obj_nr.freq_band_ind_nr         = meas_obj_nr.freq_band_ind_nr.value();
  }
  // meas cycle scell
  if (meas_obj_nr.meas_cycle_scell.has_value()) {
    asn1_meas_obj_nr.ext                      = true;
    asn1_meas_obj_nr.meas_cycle_scell_present = true;
    asn1::number_to_enum(asn1_meas_obj_nr.meas_cycle_scell, meas_obj_nr.meas_cycle_scell.value());
  }

  return asn1_meas_obj_nr;
}

template <class asn1_srs_periodicity_and_offset>
void srsran::srs_cu_cp::srs_periodicity_and_offset_to_rrc_asn1(
    asn1_srs_periodicity_and_offset&      asn1_srs_period_and_offset,
    const rrc_srs_periodicity_and_offset& srs_period_and_offset)
{
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms1) {
    asn1_srs_period_and_offset.set_sl1();
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms2) {
    asn1_srs_period_and_offset.set_sl2() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms4) {
    asn1_srs_period_and_offset.set_sl4() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms5) {
    asn1_srs_period_and_offset.set_sl5() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms8) {
    asn1_srs_period_and_offset.set_sl8() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms10) {
    asn1_srs_period_and_offset.set_sl10() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms16) {
    asn1_srs_period_and_offset.set_sl16() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms20) {
    asn1_srs_period_and_offset.set_sl20() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms32) {
    asn1_srs_period_and_offset.set_sl32() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms40) {
    asn1_srs_period_and_offset.set_sl40() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms64) {
    asn1_srs_period_and_offset.set_sl64() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms80) {
    asn1_srs_period_and_offset.set_sl80() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms160) {
    asn1_srs_period_and_offset.set_sl160() = static_cast<uint8_t>(srs_period_and_offset.offset);
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms320) {
    asn1_srs_period_and_offset.set_sl320() = srs_period_and_offset.offset;
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms640) {
    asn1_srs_period_and_offset.set_sl640() = srs_period_and_offset.offset;
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms1280) {
    asn1_srs_period_and_offset.set_sl1280() = srs_period_and_offset.offset;
  }
  if (srs_period_and_offset.period == rrc_srs_periodicity_and_offset::period_t::ms2560) {
    asn1_srs_period_and_offset.set_sl2560() = srs_period_and_offset.offset;
  }
}

asn1::rrc_nr::srs_res_s srsran::srs_cu_cp::srs_res_to_rrc_asn1(const rrc_srs_res& srs_res)
{
  asn1::rrc_nr::srs_res_s asn1_srs_res;

  // srs res id
  asn1_srs_res.srs_res_id = srs_res.srs_res_id;

  // nrof srs ports
  asn1::number_to_enum(asn1_srs_res.nrof_srs_ports, srs_res.nrof_srs_ports);

  // ptrs port idx
  if (srs_res.ptrs_port_idx.has_value()) {
    asn1_srs_res.ptrs_port_idx_present = true;
    asn1::number_to_enum(asn1_srs_res.ptrs_port_idx, srs_res.ptrs_port_idx.value());
  }

  // tx comb
  if (const auto* n2 = std::get_if<rrc_n2>(&srs_res.tx_comb); n2 != nullptr) {
    asn1_srs_res.tx_comb.set_n2();
    asn1_srs_res.tx_comb.n2().comb_offset_n2  = n2->comb_offset_n2;
    asn1_srs_res.tx_comb.n2().cyclic_shift_n2 = n2->cyclic_shift_n2;
  }

  if (const auto* n4 = std::get_if<rrc_n4>(&srs_res.tx_comb); n4 != nullptr) {
    asn1_srs_res.tx_comb.set_n4();
    asn1_srs_res.tx_comb.n4().comb_offset_n4  = n4->comb_offset_n4;
    asn1_srs_res.tx_comb.n4().cyclic_shift_n4 = n4->cyclic_shift_n4;
  }

  // res map
  asn1_srs_res.res_map.start_position = srs_res.res_map.start_position;
  asn1::number_to_enum(asn1_srs_res.res_map.nrof_symbols, srs_res.res_map.nrof_symbols);
  asn1::number_to_enum(asn1_srs_res.res_map.repeat_factor, srs_res.res_map.repeat_factor);

  // freq domain position
  asn1_srs_res.freq_domain_position = srs_res.freq_domain_position;

  // freq domain shift
  asn1_srs_res.freq_domain_shift = srs_res.freq_domain_shift;

  // freq hop
  asn1_srs_res.freq_hop.c_srs = srs_res.freq_hop.c_srs;
  asn1_srs_res.freq_hop.b_srs = srs_res.freq_hop.b_srs;
  asn1_srs_res.freq_hop.b_hop = srs_res.freq_hop.b_hop;

  // group or seq hop
  asn1::string_to_enum(asn1_srs_res.group_or_seq_hop, srs_res.group_or_seq_hop);

  // res type
  if (std::holds_alternative<bool>(srs_res.res_type)) {
    asn1_srs_res.res_type.set_aperiodic();
  }
  if (std::holds_alternative<rrc_semi_persistent>(srs_res.res_type)) {
    asn1_srs_res.res_type.set_semi_persistent();
    srs_periodicity_and_offset_to_rrc_asn1(asn1_srs_res.res_type.semi_persistent().periodicity_and_offset_sp,
                                           std::get<rrc_semi_persistent>(srs_res.res_type).periodicity_and_offset_sp_p);
  }
  if (std::holds_alternative<rrc_periodic>(srs_res.res_type)) {
    asn1_srs_res.res_type.set_periodic();
    srs_periodicity_and_offset_to_rrc_asn1(asn1_srs_res.res_type.periodic().periodicity_and_offset_p,
                                           std::get<rrc_periodic>(srs_res.res_type).periodicity_and_offset_sp_p);
  }
  // seq id
  asn1_srs_res.seq_id = srs_res.seq_id;

  // spatial relation info
  if (srs_res.spatial_relation_info.has_value()) {
    // serving cell id
    if (srs_res.spatial_relation_info.value().serving_cell_id.has_value()) {
      asn1_srs_res.spatial_relation_info.serving_cell_id_present = true;
      asn1_srs_res.spatial_relation_info.serving_cell_id =
          srs_res.spatial_relation_info.value().serving_cell_id.value();
    }
    // ref sig
    if (std::holds_alternative<rrc_srs_spatial_relation_info::ssb_idx_t>(
            srs_res.spatial_relation_info.value().ref_sig)) {
      asn1_srs_res.spatial_relation_info.ref_sig.set_ssb_idx() = static_cast<uint8_t>(
          std::get<rrc_srs_spatial_relation_info::ssb_idx_t>(srs_res.spatial_relation_info.value().ref_sig));
    }
    if (std::holds_alternative<rrc_srs_spatial_relation_info::csi_rs_idx_t>(
            srs_res.spatial_relation_info.value().ref_sig)) {
      asn1_srs_res.spatial_relation_info.ref_sig.set_csi_rs_idx() = static_cast<uint8_t>(
          std::get<rrc_srs_spatial_relation_info::csi_rs_idx_t>(srs_res.spatial_relation_info.value().ref_sig));
    }
    if (std::holds_alternative<rrc_srs>(srs_res.spatial_relation_info.value().ref_sig)) {
      asn1_srs_res.spatial_relation_info.ref_sig.set_srs();
      asn1_srs_res.spatial_relation_info.ref_sig.srs().res_id =
          std::get<rrc_srs>(srs_res.spatial_relation_info.value().ref_sig).res_id;
      asn1_srs_res.spatial_relation_info.ref_sig.srs().ul_bwp =
          std::get<rrc_srs>(srs_res.spatial_relation_info.value().ref_sig).ul_bwp;
    }
  }

  return asn1_srs_res;
}

asn1::rrc_nr::meas_obj_to_add_mod_s
srsran::srs_cu_cp::meas_obj_to_add_mod_to_rrc_asn1(const rrc_meas_obj_to_add_mod& meas_obj_to_add_mod)
{
  asn1::rrc_nr::meas_obj_to_add_mod_s asn1_meas_obj_to_add_mod;

  // meas obj id
  asn1_meas_obj_to_add_mod.meas_obj_id = meas_obj_id_to_uint(meas_obj_to_add_mod.meas_obj_id);

  // meas obj
  if (meas_obj_to_add_mod.meas_obj_nr.has_value()) {
    // meas obj nr
    asn1_meas_obj_to_add_mod.meas_obj.set_meas_obj_nr();
    asn1_meas_obj_to_add_mod.meas_obj.meas_obj_nr() = meas_obj_nr_to_rrc_asn1(meas_obj_to_add_mod.meas_obj_nr.value());
  }

  return asn1_meas_obj_to_add_mod;
}

asn1::rrc_nr::meas_report_quant_s
srsran::srs_cu_cp::meas_report_quant_to_rrc_asn1(const rrc_meas_report_quant& meas_report_quant)
{
  asn1::rrc_nr::meas_report_quant_s asn1_meas_report_quant;

  asn1_meas_report_quant.rsrp = meas_report_quant.rsrp;
  asn1_meas_report_quant.rsrq = meas_report_quant.rsrq;
  asn1_meas_report_quant.sinr = meas_report_quant.sinr;

  return asn1_meas_report_quant;
}

asn1::rrc_nr::nr_rs_type_e srsran::srs_cu_cp::rrc_nr_rs_type_to_asn1(const rrc_nr_rs_type& rs_type)
{
  asn1::rrc_nr::nr_rs_type_e asn1_rs_type;

  switch (rs_type) {
    case rrc_nr_rs_type::ssb:
      asn1_rs_type = asn1::rrc_nr::nr_rs_type_opts::options::ssb;
      break;
    case rrc_nr_rs_type::csi_rs:
      asn1_rs_type = asn1::rrc_nr::nr_rs_type_opts::options::csi_rs;
      break;
  }

  return asn1_rs_type;
}

asn1::rrc_nr::report_interv_e srsran::srs_cu_cp::report_interval_to_asn1(uint32_t report_interval)
{
  asn1::rrc_nr::report_interv_e asn1_report_interval;

  return asn1_report_interval;
}

asn1::rrc_nr::periodical_report_cfg_s
srsran::srs_cu_cp::periodical_report_cfg_to_rrc_asn1(const rrc_periodical_report_cfg& periodical_report_cfg)
{
  asn1::rrc_nr::periodical_report_cfg_s asn1_periodical_report_cfg;

  // rs type
  asn1_periodical_report_cfg.rs_type = rrc_nr_rs_type_to_asn1(periodical_report_cfg.rs_type);
  // report interv. This struct mixes ms and minutes so we need to convert the value before conversion
  if (periodical_report_cfg.report_interv < 60000) {
    asn1::number_to_enum(asn1_periodical_report_cfg.report_interv, periodical_report_cfg.report_interv);
  } else {
    asn1::number_to_enum(asn1_periodical_report_cfg.report_interv, periodical_report_cfg.report_interv / 60000);
  }

  // report amount
  asn1::number_to_enum(asn1_periodical_report_cfg.report_amount, periodical_report_cfg.report_amount);
  // report quant cell
  asn1_periodical_report_cfg.report_quant_cell = meas_report_quant_to_rrc_asn1(periodical_report_cfg.report_quant_cell);
  // max report cells
  asn1_periodical_report_cfg.max_report_cells = periodical_report_cfg.max_report_cells;
  // report quant rx idxes
  if (periodical_report_cfg.report_quant_rs_idxes.has_value()) {
    asn1_periodical_report_cfg.report_quant_rs_idxes_present = true;
    asn1_periodical_report_cfg.report_quant_rs_idxes =
        meas_report_quant_to_rrc_asn1(periodical_report_cfg.report_quant_rs_idxes.value());
  }
  // max nrof rs idxes to report
  if (periodical_report_cfg.max_nrof_rs_idxes_to_report.has_value()) {
    asn1_periodical_report_cfg.max_nrof_rs_idxes_to_report_present = true;
    asn1_periodical_report_cfg.max_nrof_rs_idxes_to_report = periodical_report_cfg.max_nrof_rs_idxes_to_report.value();
  }
  // include beam meass
  asn1_periodical_report_cfg.include_beam_meass = periodical_report_cfg.include_beam_meass;
  // use allowed cell list
  asn1_periodical_report_cfg.use_allowed_cell_list = periodical_report_cfg.use_allowed_cell_list;

  return asn1_periodical_report_cfg;
}

template <class asn1_meas_trigger_quant_quant_offset>
void srsran::srs_cu_cp::meas_trigger_quant_to_rrc_asn1(
    asn1_meas_trigger_quant_quant_offset& asn1_meas_trigger_quant_offset,
    const rrc_meas_trigger_quant&         meas_trigger_quant)
{
  if (meas_trigger_quant.rsrp.has_value()) {
    asn1_meas_trigger_quant_offset.set_rsrp() = meas_trigger_quant.rsrp.value();
  } else if (meas_trigger_quant.rsrq.has_value()) {
    asn1_meas_trigger_quant_offset.set_rsrq() = meas_trigger_quant.rsrq.value();
  } else if (meas_trigger_quant.sinr.has_value()) {
    asn1_meas_trigger_quant_offset.set_sinr() = meas_trigger_quant.sinr.value();
  } else {
    // error
    report_fatal_error("Cannot convert meas trigger quant to ASN.1 type");
  }
}

asn1::rrc_nr::event_trigger_cfg_s
srsran::srs_cu_cp::event_triggered_report_cfg_to_rrc_asn1(const rrc_event_trigger_cfg& event_trigger_cfg)
{
  asn1::rrc_nr::event_trigger_cfg_s asn1_event_trigger_cfg = {};

  // report add neigh meas present
  asn1_event_trigger_cfg.report_add_neigh_meas_present = event_trigger_cfg.report_add_neigh_meas_present;

  const auto& event_id = event_trigger_cfg.event_id;

  // event a1
  if (event_id.id == rrc_event_id::event_id_t::a1) {
    auto& asn1_event_a1 = asn1_event_trigger_cfg.event_id.set_event_a1();
    meas_trigger_quant_to_rrc_asn1(asn1_event_a1.a1_thres, event_id.meas_trigger_quant_thres_or_offset.value());
    asn1_event_a1.report_on_leave = event_id.report_on_leave;
    asn1_event_a1.hysteresis      = event_id.hysteresis;
    asn1::number_to_enum(asn1_event_a1.time_to_trigger, event_id.time_to_trigger);
  }

  // event a2
  if (event_id.id == rrc_event_id::event_id_t::a2) {
    auto& asn1_event_a2 = asn1_event_trigger_cfg.event_id.set_event_a2();
    meas_trigger_quant_to_rrc_asn1(asn1_event_a2.a2_thres, event_id.meas_trigger_quant_thres_or_offset.value());
    asn1_event_a2.report_on_leave = event_id.report_on_leave;
    asn1_event_a2.hysteresis      = event_id.hysteresis;
    asn1::number_to_enum(asn1_event_a2.time_to_trigger, event_id.time_to_trigger);
  }

  // event a3
  if (event_id.id == rrc_event_id::event_id_t::a3) {
    auto& asn1_event_a3 = asn1_event_trigger_cfg.event_id.set_event_a3();
    meas_trigger_quant_to_rrc_asn1(asn1_event_a3.a3_offset, event_id.meas_trigger_quant_thres_or_offset.value());
    asn1_event_a3.report_on_leave = event_id.report_on_leave;
    asn1_event_a3.hysteresis      = event_id.hysteresis;
    asn1::number_to_enum(asn1_event_a3.time_to_trigger, event_id.time_to_trigger);
    if (event_id.use_allowed_cell_list.has_value()) {
      asn1_event_a3.use_allowed_cell_list = event_id.use_allowed_cell_list.value();
    } else {
      asn1_event_a3.use_allowed_cell_list = false;
    }
  }

  // event a4
  if (event_id.id == rrc_event_id::event_id_t::a4) {
    auto& asn1_event_a4 = asn1_event_trigger_cfg.event_id.set_event_a4();
    meas_trigger_quant_to_rrc_asn1(asn1_event_a4.a4_thres, event_id.meas_trigger_quant_thres_or_offset.value());
    asn1_event_a4.report_on_leave = event_id.report_on_leave;
    asn1_event_a4.hysteresis      = event_id.hysteresis;
    asn1::number_to_enum(asn1_event_a4.time_to_trigger, event_id.time_to_trigger);
    if (event_id.use_allowed_cell_list.has_value()) {
      asn1_event_a4.use_allowed_cell_list = event_id.use_allowed_cell_list.value();
    } else {
      asn1_event_a4.use_allowed_cell_list = false;
    }
  }

  // event a5
  if (event_id.id == rrc_event_id::event_id_t::a5) {
    auto& asn1_event_a5 = asn1_event_trigger_cfg.event_id.set_event_a5();
    meas_trigger_quant_to_rrc_asn1(asn1_event_a5.a5_thres1, event_id.meas_trigger_quant_thres_or_offset.value());
    meas_trigger_quant_to_rrc_asn1(asn1_event_a5.a5_thres2, event_id.meas_trigger_quant_thres_or_offset.value());
    asn1_event_a5.report_on_leave = event_id.report_on_leave;
    asn1_event_a5.hysteresis      = event_id.hysteresis;
    asn1::number_to_enum(asn1_event_a5.time_to_trigger, event_id.time_to_trigger);
    if (event_id.use_allowed_cell_list.has_value()) {
      asn1_event_a5.use_allowed_cell_list = event_id.use_allowed_cell_list.value();
    } else {
      asn1_event_a5.use_allowed_cell_list = false;
    }
  }

  // event a6
  if (event_id.id == rrc_event_id::event_id_t::a6) {
    auto& asn1_event_a6 = asn1_event_trigger_cfg.event_id.set_event_a6();
    meas_trigger_quant_to_rrc_asn1(asn1_event_a6.a6_offset, event_id.meas_trigger_quant_thres_or_offset.value());
    asn1_event_a6.report_on_leave = event_id.report_on_leave;
    asn1_event_a6.hysteresis      = event_id.hysteresis;
    asn1::number_to_enum(asn1_event_a6.time_to_trigger, event_id.time_to_trigger);
    if (event_id.use_allowed_cell_list.has_value()) {
      asn1_event_a6.use_allowed_cell_list = event_id.use_allowed_cell_list.value();
    } else {
      asn1_event_a6.use_allowed_cell_list = false;
    }
  }

  // rs type
  asn1_event_trigger_cfg.rs_type = rrc_nr_rs_type_to_asn1(event_trigger_cfg.rs_type);

  // report interv
  // report interv. This struct mixes ms and minutes so we need to convert the value before conversion
  if (event_trigger_cfg.report_interv < 60000) {
    asn1::number_to_enum(asn1_event_trigger_cfg.report_interv, event_trigger_cfg.report_interv);
  } else {
    asn1::number_to_enum(asn1_event_trigger_cfg.report_interv, event_trigger_cfg.report_interv / 60000);
  }

  // report amount
  asn1::number_to_enum(asn1_event_trigger_cfg.report_amount, event_trigger_cfg.report_amount);

  // report quant cell
  asn1_event_trigger_cfg.report_quant_cell = meas_report_quant_to_rrc_asn1(event_trigger_cfg.report_quant_cell);

  // max report cells
  asn1_event_trigger_cfg.max_report_cells = event_trigger_cfg.max_report_cells;

  // report quant rx idxes
  if (event_trigger_cfg.report_quant_rs_idxes.has_value()) {
    asn1_event_trigger_cfg.report_quant_rs_idxes_present = true;
    asn1_event_trigger_cfg.report_quant_rs_idxes =
        meas_report_quant_to_rrc_asn1(event_trigger_cfg.report_quant_rs_idxes.value());
  }

  // max nrof rs idxes to report
  if (event_trigger_cfg.max_nrof_rs_idxes_to_report.has_value()) {
    asn1_event_trigger_cfg.max_nrof_rs_idxes_to_report_present = true;
    asn1_event_trigger_cfg.max_nrof_rs_idxes_to_report         = event_trigger_cfg.max_nrof_rs_idxes_to_report.value();
  }

  // include beam meass
  asn1_event_trigger_cfg.include_beam_meass = event_trigger_cfg.include_beam_meass;

  return asn1_event_trigger_cfg;
}

asn1::rrc_nr::report_cfg_nr_s srsran::srs_cu_cp::report_cfg_nr_to_rrc_asn1(const rrc_report_cfg_nr& report_cfg_nr)
{
  asn1::rrc_nr::report_cfg_nr_s asn1_report_cfg_nr;

  if (const auto* periodical = std::get_if<rrc_periodical_report_cfg>(&report_cfg_nr); periodical != nullptr) {
    // periodical
    asn1_report_cfg_nr.report_type.set_periodical() = periodical_report_cfg_to_rrc_asn1(*periodical);
  }

  if (const auto* event_triggered = std::get_if<rrc_event_trigger_cfg>(&report_cfg_nr); event_triggered != nullptr) {
    // event triggered
    asn1_report_cfg_nr.report_type.set_event_triggered() = event_triggered_report_cfg_to_rrc_asn1(*event_triggered);
  }

  if (const auto* report_cgi = std::get_if<rrc_report_cgi>(&report_cfg_nr); report_cgi != nullptr) {
    // report cgi
    asn1_report_cfg_nr.report_type.set_report_cgi();
    asn1_report_cfg_nr.report_type.report_cgi().cell_for_which_to_report_cgi = report_cgi->cell_for_which_to_report_cgi;
  }

  if (const auto* report_sftd = std::get_if<rrc_report_sftd_nr>(&report_cfg_nr); report_sftd != nullptr) {
    // report sftd
    asn1_report_cfg_nr.report_type.set_report_sftd();
    asn1_report_cfg_nr.report_type.report_sftd().report_sftd_meas = report_sftd->report_sftd_meas;
    asn1_report_cfg_nr.report_type.report_sftd().report_rsrp      = report_sftd->report_rsrp;
  }

  return asn1_report_cfg_nr;
}

asn1::rrc_nr::report_cfg_to_add_mod_s
srsran::srs_cu_cp::report_cfg_to_add_mod_to_rrc_asn1(const rrc_report_cfg_to_add_mod& report_cfg_to_add_mod)
{
  asn1::rrc_nr::report_cfg_to_add_mod_s asn1_report_cfg_to_add_mod;

  // report cfg id
  asn1_report_cfg_to_add_mod.report_cfg_id = report_cfg_id_to_uint(report_cfg_to_add_mod.report_cfg_id);

  // report cfg
  asn1_report_cfg_to_add_mod.report_cfg.set_report_cfg_nr() =
      report_cfg_nr_to_rrc_asn1(report_cfg_to_add_mod.report_cfg);

  return asn1_report_cfg_to_add_mod;
}

asn1::rrc_nr::meas_id_to_add_mod_s
srsran::srs_cu_cp::meas_id_to_add_mod_to_rrc_asn1(const rrc_meas_id_to_add_mod& meas_id_to_add_mod)
{
  asn1::rrc_nr::meas_id_to_add_mod_s asn1_meas_id_to_add_mod;

  // meas id
  asn1_meas_id_to_add_mod.meas_id = meas_id_to_uint(meas_id_to_add_mod.meas_id);
  // meas obj id
  asn1_meas_id_to_add_mod.meas_obj_id = meas_obj_id_to_uint(meas_id_to_add_mod.meas_obj_id);
  // report cfg ind
  asn1_meas_id_to_add_mod.report_cfg_id = report_cfg_id_to_uint(meas_id_to_add_mod.report_cfg_id);

  return asn1_meas_id_to_add_mod;
}

asn1::rrc_nr::filt_cfg_s srsran::srs_cu_cp::filt_cfg_to_rrc_asn1(const rrc_filt_cfg& filt_cfg)
{
  asn1::rrc_nr::filt_cfg_s asn1_filt_cfg;

  // filt coef rsrp
  if (filt_cfg.filt_coef_rsrp.has_value()) {
    asn1_filt_cfg.filt_coef_rsrp_present = true;
    asn1::number_to_enum(asn1_filt_cfg.filt_coef_rsrp, filt_cfg.filt_coef_rsrp.value());
  }

  // filt coef rsrq
  if (filt_cfg.filt_coef_rsrq.has_value()) {
    asn1_filt_cfg.filt_coef_rsrq_present = true;
    asn1::number_to_enum(asn1_filt_cfg.filt_coef_rsrq, filt_cfg.filt_coef_rsrq.value());
  }

  // filt coef rs sinr
  if (filt_cfg.filt_coef_rs_sinr.has_value()) {
    asn1_filt_cfg.filt_coef_rs_sinr_present = true;
    asn1::number_to_enum(asn1_filt_cfg.filt_coef_rs_sinr, filt_cfg.filt_coef_rs_sinr.value());
  }

  return asn1_filt_cfg;
}

asn1::rrc_nr::quant_cfg_rs_s srsran::srs_cu_cp::quant_cfg_rs_to_rrc_asn1(const rrc_quant_cfg_rs& quant_cfg_rs)
{
  asn1::rrc_nr::quant_cfg_rs_s asn1_quant_cfg_rs;

  // ssb filt cfg
  asn1_quant_cfg_rs.ssb_filt_cfg = filt_cfg_to_rrc_asn1(quant_cfg_rs.ssb_filt_cfg);
  // csi rs filt cfg
  asn1_quant_cfg_rs.csi_rs_filt_cfg = filt_cfg_to_rrc_asn1(quant_cfg_rs.csi_rs_filt_cfg);

  return asn1_quant_cfg_rs;
}

asn1::rrc_nr::meas_cfg_s srsran::srs_cu_cp::meas_config_to_rrc_asn1(const rrc_meas_cfg& meas_cfg)
{
  asn1::rrc_nr::meas_cfg_s asn1_meas_cfg;

  // meas obj to rem list
  for (const auto& meas_obj_to_rem : meas_cfg.meas_obj_to_rem_list) {
    asn1_meas_cfg.meas_obj_to_rem_list.push_back(meas_obj_id_to_uint(meas_obj_to_rem));
  }

  // meas obj to add mod list
  for (const auto& meas_obj_to_add_mod : meas_cfg.meas_obj_to_add_mod_list) {
    asn1::rrc_nr::meas_obj_to_add_mod_s asn1_meas_obj_to_add_mod = meas_obj_to_add_mod_to_rrc_asn1(meas_obj_to_add_mod);
    asn1_meas_cfg.meas_obj_to_add_mod_list.push_back(asn1_meas_obj_to_add_mod);
  }

  // report cfg to rem list
  for (const auto& report_cfg_to_rem : meas_cfg.report_cfg_to_rem_list) {
    asn1_meas_cfg.report_cfg_to_rem_list.push_back(report_cfg_id_to_uint(report_cfg_to_rem));
  }

  // report cfg to add mod list
  for (const auto& report_cfg_to_add_mod : meas_cfg.report_cfg_to_add_mod_list) {
    asn1::rrc_nr::report_cfg_to_add_mod_s asn1_report_cfg_to_add_mod =
        report_cfg_to_add_mod_to_rrc_asn1(report_cfg_to_add_mod);
    asn1_meas_cfg.report_cfg_to_add_mod_list.push_back(asn1_report_cfg_to_add_mod);
  }

  // meas id to rem list
  for (const auto& meas_id_to_rem : meas_cfg.meas_id_to_rem_list) {
    asn1_meas_cfg.meas_id_to_rem_list.push_back(meas_id_to_uint(meas_id_to_rem));
  }

  // meas id to add mod list
  for (const auto& meas_id_to_add_mod : meas_cfg.meas_id_to_add_mod_list) {
    asn1::rrc_nr::meas_id_to_add_mod_s asn1_meas_id_to_add_mod = meas_id_to_add_mod_to_rrc_asn1(meas_id_to_add_mod);
    asn1_meas_cfg.meas_id_to_add_mod_list.push_back(asn1_meas_id_to_add_mod);
  }

  // s measure cfg
  if (meas_cfg.s_measure_cfg.has_value()) {
    asn1_meas_cfg.s_measure_cfg_present = true;
    if (meas_cfg.s_measure_cfg.value().type == rrc_s_measure_cfg::measure_type_t::ssb_rsrp) {
      asn1_meas_cfg.s_measure_cfg.set_ssb_rsrp() = meas_cfg.s_measure_cfg.value().measure_config;
    }

    if (meas_cfg.s_measure_cfg.value().type == rrc_s_measure_cfg::measure_type_t::csi_rsrp) {
      asn1_meas_cfg.s_measure_cfg.set_csi_rsrp() = meas_cfg.s_measure_cfg.value().measure_config;
    }
  }

  // quant cfg
  if (meas_cfg.quant_cfg.has_value()) {
    asn1_meas_cfg.quant_cfg_present = true;
    for (const auto& quant_cfg_nr : meas_cfg.quant_cfg.value().quant_cfg_nr_list) {
      asn1::rrc_nr::quant_cfg_nr_s asn1_quant_cfg_nr;

      // quant cfg cell
      asn1_quant_cfg_nr.quant_cfg_cell = quant_cfg_rs_to_rrc_asn1(quant_cfg_nr.quant_cfg_cell);

      // quant cfg rs idx
      if (quant_cfg_nr.quant_cfg_rs_idx.has_value()) {
        asn1_quant_cfg_nr.quant_cfg_rs_idx_present = true;
        asn1_quant_cfg_nr.quant_cfg_rs_idx         = quant_cfg_rs_to_rrc_asn1(quant_cfg_nr.quant_cfg_rs_idx.value());
      }

      asn1_meas_cfg.quant_cfg.quant_cfg_nr_list.push_back(asn1_quant_cfg_nr);
    }
  }

  return asn1_meas_cfg;
}

rrc_meas_quant_results
srsran::srs_cu_cp::asn1_to_meas_quant_results(const asn1::rrc_nr::meas_quant_results_s& asn1_meas_quant_results)
{
  rrc_meas_quant_results meas_quant_results;

  // rsrp
  if (asn1_meas_quant_results.rsrp_present) {
    meas_quant_results.rsrp = asn1_meas_quant_results.rsrp;
  }

  // rsrq
  if (asn1_meas_quant_results.rsrq_present) {
    meas_quant_results.rsrq = asn1_meas_quant_results.rsrq;
  }

  // sinr
  if (asn1_meas_quant_results.sinr_present) {
    meas_quant_results.sinr = asn1_meas_quant_results.sinr;
  }

  return meas_quant_results;
}

rrc_meas_result_nr srsran::srs_cu_cp::asn1_to_meas_result_nr(const asn1::rrc_nr::meas_result_nr_s& asn1_meas_result_nr)
{
  rrc_meas_result_nr meas_result_nr;

  // pci
  if (asn1_meas_result_nr.pci_present) {
    meas_result_nr.pci = asn1_meas_result_nr.pci;
  }

  // cell results
  // results ssb cell
  if (asn1_meas_result_nr.meas_result.cell_results.results_ssb_cell_present) {
    meas_result_nr.cell_results.results_ssb_cell =
        asn1_to_meas_quant_results(asn1_meas_result_nr.meas_result.cell_results.results_ssb_cell);
  }
  // results csi rs cell
  if (asn1_meas_result_nr.meas_result.cell_results.results_csi_rs_cell_present) {
    meas_result_nr.cell_results.results_csi_rs_cell =
        asn1_to_meas_quant_results(asn1_meas_result_nr.meas_result.cell_results.results_csi_rs_cell);
  }

  // rs idx results
  if (asn1_meas_result_nr.meas_result.rs_idx_results_present) {
    rrc_meas_result_nr::rs_idx_results_ rs_idx_result;

    // results ssb idxes
    for (const auto& asn1_ssb_result : asn1_meas_result_nr.meas_result.rs_idx_results.results_ssb_idxes) {
      rrc_results_per_ssb_idx ssb_result;

      ssb_result.ssb_idx = asn1_ssb_result.ssb_idx;

      if (asn1_ssb_result.ssb_results_present) {
        ssb_result.ssb_results = asn1_to_meas_quant_results(asn1_ssb_result.ssb_results);
      }
      rs_idx_result.results_ssb_idxes.emplace(asn1_ssb_result.ssb_idx, ssb_result);
    }

    // results csi_rs idxes
    for (const auto& asn1_csi_rs_result : asn1_meas_result_nr.meas_result.rs_idx_results.results_csi_rs_idxes) {
      rrc_results_per_csi_rs_idx csi_rs_result;
      if (asn1_csi_rs_result.csi_rs_results_present) {
        csi_rs_result.csi_rs_results = asn1_to_meas_quant_results(asn1_csi_rs_result.csi_rs_results);
      }
      rs_idx_result.results_csi_rs_idxes.emplace(asn1_csi_rs_result.csi_rs_idx, csi_rs_result);
    }

    meas_result_nr.rs_idx_results = rs_idx_result;
  }

  return meas_result_nr;
}

rrc_meas_results srsran::srs_cu_cp::asn1_to_measurement_results(const asn1::rrc_nr::meas_results_s& asn1_meas_results,
                                                                srslog::basic_logger&               logger)
{
  rrc_meas_results meas_results;

  // meas id
  meas_results.meas_id = uint_to_meas_id(asn1_meas_results.meas_id);

  // meas result serving mo list
  for (const auto& asn1_meas_result_serv_mo : asn1_meas_results.meas_result_serving_mo_list) {
    rrc_meas_result_serv_mo meas_result_serv_mo;

    // serv cell id
    meas_result_serv_mo.serv_cell_id = asn1_meas_result_serv_mo.serv_cell_id;
    // meas result serving cell
    meas_result_serv_mo.meas_result_serving_cell =
        asn1_to_meas_result_nr(asn1_meas_result_serv_mo.meas_result_serving_cell);
    // meas result best neigh cell
    if (asn1_meas_result_serv_mo.meas_result_best_neigh_cell_present) {
      meas_result_serv_mo.meas_result_best_neigh_cell =
          asn1_to_meas_result_nr(asn1_meas_result_serv_mo.meas_result_best_neigh_cell);
    }

    meas_results.meas_result_serving_mo_list.emplace(asn1_meas_result_serv_mo.serv_cell_id, meas_result_serv_mo);
  }

  // meas result neigh cells
  if (asn1_meas_results.meas_result_neigh_cells_present) {
    rrc_meas_result_neigh_cells meas_result_neigh_cell;

    if (asn1_meas_results.meas_result_neigh_cells.type() ==
        asn1::rrc_nr::meas_results_s::meas_result_neigh_cells_c_::types_opts::options::meas_result_list_nr) {
      // meas result list nr
      for (const auto& asn1_meas_result_nr : asn1_meas_results.meas_result_neigh_cells.meas_result_list_nr()) {
        meas_result_neigh_cell.meas_result_list_nr.push_back(asn1_to_meas_result_nr(asn1_meas_result_nr));
      }
    } else {
      // error
      logger.error("Ignoring neighbor cell measurement. Cause: Unsupported cell type {}",
                   asn1_meas_results.meas_result_neigh_cells.type().to_string());
    }

    meas_results.meas_result_neigh_cells = meas_result_neigh_cell;
  }

  return meas_results;
}
