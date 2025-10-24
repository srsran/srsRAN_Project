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

#include "srsran/scheduler/config/csi_helper.h"

using namespace srsran;
using namespace csi_helper;

/// Get CRBs across which a CSI resource spans as per TS 38.331, "CSI-FrequencyOccupation".
///
/// \param nof_crbs Number of CRBs used for the CSI-RS.
/// \param bwp_nof_rbs Width of the BWP where the CSI-RS is used, in RBs.
/// \return A valid number of RBs for the CSI-RS.
static crb_interval get_csi_freq_occupation_rbs(unsigned nof_crbs, unsigned bwp_nof_rbs)
{
  // - Only multiples of 4 are allowed.
  // - The smallest configurable number is the MIN(24, BWP_width).
  // Note: If the configured value is larger than the width of the corresponding BWP, the UE shall assume that the
  // actual CSI-RS bandwidth is equal to the width of the BWP.
  const unsigned nof_rbs_in_multiples_of_4 = 4 * divide_ceil(nof_crbs, 4);

  return {0, std::max(nof_rbs_in_multiples_of_4, std::min(24U, bwp_nof_rbs))};
}

/// \brief Compute default CSI-RS signalling period to use, while constrained by TS 38.214, 5.1.6.1.1.
csi_resource_periodicity srsran::csi_helper::get_max_csi_rs_period(subcarrier_spacing pdsch_scs)
{
  const csi_resource_periodicity max_csi_period =
      static_cast<csi_resource_periodicity>(std::min(80U * get_nof_slots_per_subframe(pdsch_scs), 640U));
  return max_csi_period;
}

bool srsran::csi_helper::is_csi_rs_period_valid(csi_resource_periodicity       csi_rs_period,
                                                const tdd_ul_dl_config_common& tdd_cfg)
{
  // The CSI-RS period must be multiple of the TDD period.
  const unsigned tdd_period = nof_slots_per_tdd_period(tdd_cfg);
  if (static_cast<unsigned>(csi_rs_period) % tdd_period != 0) {
    return false;
  }

  // According to TS 38.214, Section 5.1.6.1.1, a UE expects periods of 10, 20, 40, or 80 milliseconds in CSI-RS for
  // tracking.
  unsigned                      nof_slots_per_subframe = get_nof_slots_per_subframe(tdd_cfg.ref_scs);
  const std::array<unsigned, 4> csi_opt_msec           = {10 * nof_slots_per_subframe,
                                                          20 * nof_slots_per_subframe,
                                                          40 * nof_slots_per_subframe,
                                                          80 * nof_slots_per_subframe};

  return std::find(csi_opt_msec.begin(), csi_opt_msec.end(), csi_resource_periodicity_to_uint(csi_rs_period)) !=
         csi_opt_msec.end();
}

std::optional<csi_resource_periodicity>
srsran::csi_helper::find_valid_csi_rs_period(const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned tdd_period = nof_slots_per_tdd_period(tdd_cfg);

  csi_resource_periodicity csi_rs_period = get_max_csi_rs_period(tdd_cfg.ref_scs);
  if (static_cast<unsigned>(csi_rs_period) % tdd_period == 0) {
    return csi_rs_period;
  }

  span<const csi_resource_periodicity> csi_options = csi_resource_periodicity_options();
  auto                                 rit         = std::find(csi_options.rbegin(), csi_options.rend(), csi_rs_period);
  auto found_rit = std::find_if(++rit, csi_options.rend(), [tdd_period](csi_resource_periodicity period) {
    return static_cast<unsigned>(period) % tdd_period == 0;
  });
  if (found_rit == csi_options.rend()) {
    return std::nullopt;
  }
  return *rit;
}

// Verifies whether a CSI-RS slot offset falls in an UL slot and does not collide with SIB1 and SSB.
static bool is_csi_slot_offset_valid(unsigned                       slot_offset,
                                     const tdd_ul_dl_config_common& tdd_cfg,
                                     unsigned                       max_csi_symbol_index,
                                     unsigned                       ssb_period_ms)
{
  // TODO: this is true for SSB pattern 1000 or 10000000.
  static constexpr unsigned SSB_SLOT = 0U;
  // TODO: import sib1_period from expert config and SIB1_OFFSET from SIB1 config.
  static constexpr unsigned SIB1_PERIOD_MS = 160, SIB1_OFFSET = 1;

  const unsigned ssb_period_slots  = ssb_period_ms * get_nof_slots_per_subframe(tdd_cfg.ref_scs);
  const unsigned sib1_period_slots = SIB1_PERIOD_MS * get_nof_slots_per_subframe(tdd_cfg.ref_scs);

  // TODO: Use non-hard coded values.
  if ((slot_offset % ssb_period_slots == SSB_SLOT) or (slot_offset % sib1_period_slots == SIB1_OFFSET)) {
    return false;
  }

  const unsigned slot_index = slot_offset % (get_nof_slots_per_subframe(tdd_cfg.ref_scs) * NOF_SUBFRAMES_PER_FRAME);
  return get_active_tdd_dl_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() > max_csi_symbol_index;
}

bool srsran::csi_helper::derive_valid_csi_rs_slot_offsets(csi_builder_params&            csi_params,
                                                          const std::optional<unsigned>& meas_csi_slot_offset,
                                                          const std::optional<unsigned>& tracking_csi_slot_offset,
                                                          const std::optional<unsigned>& zp_csi_slot_offset,
                                                          const tdd_ul_dl_config_common& tdd_cfg,
                                                          unsigned                       max_csi_symbol_index,
                                                          unsigned                       ssb_period_ms)
{
  srsran_assert(is_csi_rs_period_valid(csi_params.csi_rs_period, tdd_cfg),
                "Invalid CSI-RS period {} for provided TDD pattern",
                fmt::underlying(csi_params.csi_rs_period));

  // Fill the pre-specified parameters and verify if valid.
  if (meas_csi_slot_offset.has_value()) {
    if (not is_csi_slot_offset_valid(*meas_csi_slot_offset, tdd_cfg, max_csi_symbol_index, ssb_period_ms)) {
      return false;
    }
    csi_params.meas_csi_slot_offset = *meas_csi_slot_offset;
  }
  if (tracking_csi_slot_offset.has_value()) {
    // Tracking CSI-RS uses two consecutive slots.
    if (not is_csi_slot_offset_valid(*tracking_csi_slot_offset, tdd_cfg, max_csi_symbol_index, ssb_period_ms) or
        not is_csi_slot_offset_valid(*tracking_csi_slot_offset + 1, tdd_cfg, max_csi_symbol_index, ssb_period_ms)) {
      return false;
    }
    csi_params.tracking_csi_slot_offset = *tracking_csi_slot_offset;
  }
  if (zp_csi_slot_offset.has_value()) {
    if (not is_csi_slot_offset_valid(*zp_csi_slot_offset, tdd_cfg, max_csi_symbol_index, ssb_period_ms)) {
      return false;
    }
    csi_params.zp_csi_slot_offset = *zp_csi_slot_offset;
  }

  // Make slot offset the same for IM and measurement by default.
  if (meas_csi_slot_offset.has_value() and not zp_csi_slot_offset.has_value()) {
    csi_params.zp_csi_slot_offset = *meas_csi_slot_offset;
  }
  if (not meas_csi_slot_offset.has_value() and zp_csi_slot_offset.has_value()) {
    csi_params.meas_csi_slot_offset = *zp_csi_slot_offset;
  }

  bool tracking_found = tracking_csi_slot_offset.has_value();
  bool meas_found     = meas_csi_slot_offset.has_value() or zp_csi_slot_offset.has_value();
  for (unsigned i = 0; i < static_cast<unsigned>(csi_params.csi_rs_period) and (not meas_found or not tracking_found);
       ++i) {
    if (not is_csi_slot_offset_valid(i, tdd_cfg, max_csi_symbol_index, ssb_period_ms)) {
      continue;
    }
    // Note: Tracking CSI-RS occupies two consecutive slots.
    if (not tracking_found and is_csi_slot_offset_valid(i + 1, tdd_cfg, max_csi_symbol_index, ssb_period_ms) and
        (not meas_found or (i != csi_params.meas_csi_slot_offset and (i + 1) != csi_params.meas_csi_slot_offset and
                            i != csi_params.zp_csi_slot_offset and (i + 1) != csi_params.zp_csi_slot_offset))) {
      tracking_found                      = true;
      csi_params.tracking_csi_slot_offset = i;
      i++;
    }

    if (not meas_found and (not tracking_found or (i != csi_params.tracking_csi_slot_offset and
                                                   (i != csi_params.tracking_csi_slot_offset + 1)))) {
      meas_found                      = true;
      csi_params.meas_csi_slot_offset = i;
      csi_params.zp_csi_slot_offset   = i;
    }
  }

  return meas_found and tracking_found;
}

static zp_csi_rs_resource make_default_zp_csi_rs_resource(const csi_builder_params& params)
{
  if (params.nof_ports > 4) {
    report_error("Unsupported number of antenna ports={}", params.nof_ports);
  }

  zp_csi_rs_resource res{};
  res.id = static_cast<zp_csi_rs_res_id_t>(0);
  // [Implementation-defined] The reason for using row 4 of Table 7.4.1.5.3-1 in TS 38.221 even in case of nof. ports <
  // 4 is due to some RUs not supporting more than 1 ZP CSI-RS resource per symbol. Also, the specification does not
  // restrict from using row 4 even in case of nof. ports < 4.
  // Freq Alloc -> Row4.
  res.res_mapping.nof_ports = 4;
  res.res_mapping.fd_alloc.resize(3);
  res.res_mapping.fd_alloc.set(params.pci % res.res_mapping.fd_alloc.size(), true);
  res.res_mapping.cdm                     = csi_rs_cdm_type::fd_CDM2;
  res.res_mapping.first_ofdm_symbol_in_td = params.zp_csi_ofdm_symbol_index;
  res.res_mapping.freq_density            = csi_rs_freq_density_type::one;
  res.res_mapping.freq_band_rbs           = get_csi_freq_occupation_rbs(params.nof_rbs, params.nof_rbs);
  res.period                              = params.csi_rs_period;
  res.offset                              = 2;

  return res;
}

std::vector<zp_csi_rs_resource>
srsran::csi_helper::make_periodic_zp_csi_rs_resource_list(const csi_builder_params& params)
{
  if (params.nof_ports > 4) {
    report_error("Unsupported number of antenna ports {}", params.nof_ports);
  }

  std::vector<zp_csi_rs_resource> list;
  // 1 zp-CSI-RS resource.
  list.resize(1, make_default_zp_csi_rs_resource(params));
  list[0].id = static_cast<zp_csi_rs_res_id_t>(0);

  for (auto& res : list) {
    res.offset = params.zp_csi_slot_offset;
    res.period = params.csi_rs_period;
  }

  return list;
}

zp_csi_rs_resource_set srsran::csi_helper::make_periodic_zp_csi_rs_resource_set(const csi_builder_params& params)
{
  if (params.nof_ports > 4) {
    report_error("Unsupported number of antenna ports {}", params.nof_ports);
  }

  zp_csi_rs_resource_set zp_set{};

  zp_set.id                 = static_cast<zp_csi_rs_res_set_id_t>(0);
  zp_set.zp_csi_rs_res_list = {static_cast<zp_csi_rs_res_set_id_t>(0)};

  return zp_set;
}

/// \brief Converts a CSI resource periodicity to a suitable CSI report periodicity. Report peridiocity should be
/// larger than the resource periodicity.
static csi_report_periodicity convert_csi_resource_period_to_report_period(csi_resource_periodicity res_period)
{
  switch (res_period) {
    case csi_resource_periodicity::slots4:
      return csi_report_periodicity::slots4;
    case csi_resource_periodicity::slots5:
      return csi_report_periodicity::slots5;
    case csi_resource_periodicity::slots8:
      return csi_report_periodicity::slots8;
    case csi_resource_periodicity::slots10:
      return csi_report_periodicity::slots10;
    case csi_resource_periodicity::slots16:
      return csi_report_periodicity::slots16;
    case csi_resource_periodicity::slots20:
      return csi_report_periodicity::slots20;
    case csi_resource_periodicity::slots32:
      // fallthrough
    case csi_resource_periodicity::slots40:
      return csi_report_periodicity::slots40;
    case csi_resource_periodicity::slots64:
      // fallthrough
    case csi_resource_periodicity::slots80:
      return csi_report_periodicity::slots80;
    case csi_resource_periodicity::slots160:
      return csi_report_periodicity::slots160;
    case csi_resource_periodicity::slots320:
      // fallthrough
    case csi_resource_periodicity::slots640:
      return csi_report_periodicity::slots320;
  }
  return srsran::csi_report_periodicity::slots320;
}

// Fills the values that are common to all CSI-RS resources.
static nzp_csi_rs_resource make_common_nzp_csi_rs_resource(const csi_builder_params& params)
{
  nzp_csi_rs_resource res{};

  // Fill csi_rs_resource_mapping.
  res.res_mapping.freq_density  = csi_rs_freq_density_type::three;
  res.res_mapping.freq_band_rbs = get_csi_freq_occupation_rbs(params.nof_rbs, params.nof_rbs);

  res.pwr_ctrl_offset       = 0;
  res.pwr_ctrl_offset_ss_db = 0;
  res.scrambling_id         = params.pci;

  res.csi_res_period = params.csi_rs_period;

  res.qcl_info_periodic_csi_rs = static_cast<tci_state_id_t>(0);

  return res;
}

static nzp_csi_rs_resource make_channel_measurement_nzp_csi_rs_resource(const csi_builder_params& params)
{
  srsran_assert(params.meas_csi_slot_offset < csi_resource_periodicity_to_uint(params.csi_rs_period),
                "Invalid CSI slot offset {} >= {}",
                params.meas_csi_slot_offset,
                csi_resource_periodicity_to_uint(params.csi_rs_period));
  nzp_csi_rs_resource res = make_common_nzp_csi_rs_resource(params);

  res.res_id                              = static_cast<nzp_csi_rs_res_id_t>(0);
  res.csi_res_offset                      = params.meas_csi_slot_offset;
  res.res_mapping.first_ofdm_symbol_in_td = params.cm_csi_ofdm_symbol_index;
  res.res_mapping.nof_ports               = params.nof_ports;
  res.res_mapping.freq_density            = csi_rs_freq_density_type::one;

  // Select the amount of frequency-domain resources and the CDM configuration depending on the number of transmit
  // ports.
  if (params.nof_ports == 1) {
    // Code multiplexing is not necessary when only one port is used. This makes it possible to have twelve possible
    // frequency domain allocations.
    res.res_mapping.fd_alloc.resize(12);
    res.res_mapping.cdm = csi_rs_cdm_type::no_CDM;
  } else if (params.nof_ports == 2) {
    // Code multiplexing of two resource elements is used when two transmitted ports are used. This allows six possible
    // frequency domain locations.
    res.res_mapping.fd_alloc.resize(6);
    res.res_mapping.cdm = csi_rs_cdm_type::fd_CDM2;
  } else if (params.nof_ports == 4) {
    // Code multiplexing of four resource elements is used when four transmitted ports are used. This allows three
    // possible frequency domain locations.
    res.res_mapping.fd_alloc.resize(3);
    res.res_mapping.cdm = csi_rs_cdm_type::fd_CDM2;
  } else {
    // Another number of ports is not currently supported.
    report_error("Number of ports {} not supported", params.nof_ports);
  }

  // [Implementation-defined] Select the frequency domain allocation in function of the PCI to avoid that NZP-CSI-RS
  // from neighbor cells overlap.
  res.res_mapping.fd_alloc.set(params.pci % res.res_mapping.fd_alloc.size());

  return res;
}

/// \brief Generate Tracking Reference Signal (TRS) resource set.
///
/// The TRS resource set contains four NZP-CSI-RS resources. The resources are mapped on two consecutive slots.
///
/// The NZP-CSI-RS resources selected for TRS are constrained by TS 38.214 Section 5.1.6.1.1 which specifies the number
/// of ports, multiplexing, OFDM symbols to use within the slot, periodicity, and density.
static void
fill_tracking_nzp_csi_rs_resource(span<nzp_csi_rs_resource> tracking_csi_rs,
                                  const csi_builder_params& params,
                                  nzp_csi_rs_res_id_t       first_csi_res_id = static_cast<nzp_csi_rs_res_id_t>(1))
{
  static size_t NOF_TRACKING_RESOURCES = 4;

  srsran_assert(tracking_csi_rs.size() == NOF_TRACKING_RESOURCES, "Invalid tracking CSI-RS resource list size");
  srsran_assert(params.tracking_csi_slot_offset + 1 < csi_resource_periodicity_to_uint(params.csi_rs_period),
                "Invalid CSI slot offset");
  nzp_csi_rs_resource res = make_common_nzp_csi_rs_resource(params);

  res.res_mapping.nof_ports    = 1;
  res.res_mapping.cdm          = csi_rs_cdm_type::no_CDM;
  res.res_mapping.freq_density = csi_rs_freq_density_type::three;
  res.res_mapping.fd_alloc.resize(4);

  // [Implementation-defined] Select the frequency domain allocation in function of the PCI to avoid that NZP-CSI-RS
  // from neighbor cells overlap.
  res.res_mapping.fd_alloc.set(params.pci % res.res_mapping.fd_alloc.size());

  static constexpr unsigned rel_slot_offset[] = {0, 0, 1, 1};
  for (unsigned i = 0; i != NOF_TRACKING_RESOURCES; ++i) {
    res.res_id                              = static_cast<nzp_csi_rs_res_id_t>(first_csi_res_id + i);
    res.res_mapping.first_ofdm_symbol_in_td = params.tracking_csi_ofdm_symbol_indices[i];
    res.csi_res_offset                      = params.tracking_csi_slot_offset + rel_slot_offset[i];
    tracking_csi_rs[i]                      = res;
  }
}

std::vector<nzp_csi_rs_resource> srsran::csi_helper::make_nzp_csi_rs_resource_list(const csi_builder_params& params)
{
  std::vector<nzp_csi_rs_resource> list(5);

  // Channel Measurement - Resource 0.
  list[0]        = make_channel_measurement_nzp_csi_rs_resource(params);
  list[0].res_id = static_cast<nzp_csi_rs_res_id_t>(0);

  // Tracking - Resources 1-4.
  fill_tracking_nzp_csi_rs_resource(span<nzp_csi_rs_resource>(list).subspan(1, 4), params);

  return list;
}

static std::vector<nzp_csi_rs_resource_set> make_nzp_csi_rs_resource_sets()
{
  std::vector<nzp_csi_rs_resource_set> sets(2);

  // Resource Set 0 - Measurement.
  sets[0].res_set_id          = static_cast<nzp_csi_rs_res_set_id_t>(0);
  sets[0].nzp_csi_rs_res      = {static_cast<nzp_csi_rs_res_id_t>(0)};
  sets[0].is_trs_info_present = false;

  // Resource Set 1 - Tracking.
  sets[1].res_set_id          = static_cast<nzp_csi_rs_res_set_id_t>(1);
  sets[1].nzp_csi_rs_res      = {static_cast<nzp_csi_rs_res_id_t>(1),
                                 static_cast<nzp_csi_rs_res_id_t>(2),
                                 static_cast<nzp_csi_rs_res_id_t>(3),
                                 static_cast<nzp_csi_rs_res_id_t>(4)};
  sets[1].is_trs_info_present = true;

  return sets;
}

/// \brief Returns the subcarrier location given the bit location set in \c frequencyDomainAllocation of CSI-RS-Resource
/// and size of the \c frequencyDomainAllocation bitmap.
static unsigned get_subcarrier_location_from_fd_alloc_bit_location(int     fd_alloc_bit_location,
                                                                   uint8_t fd_alloc_bitmap_size)
{
  // See TS 38.211, clause 7.4.1.5.3.
  switch (fd_alloc_bitmap_size) {
    case 4: {
      srsran_assert(fd_alloc_bit_location < 4,
                    "Invalid bit location={} in frequency domain allocation of CSI-RS for row 1",
                    fd_alloc_bit_location);
      return fd_alloc_bit_location;
    }
    case 12: {
      srsran_assert(fd_alloc_bit_location < 12,
                    "Invalid bit location={} in frequency domain allocation of CSI-RS for row 2",
                    fd_alloc_bit_location);
      return fd_alloc_bit_location;
    }
    case 3: {
      srsran_assert(fd_alloc_bit_location < 3,
                    "Invalid bit location={} in frequency domain allocation of CSI-RS for row 4",
                    fd_alloc_bit_location);
      return 4 * fd_alloc_bit_location;
    }
    case 6: {
      srsran_assert(fd_alloc_bit_location < 6,
                    "Invalid bit location={} in frequency domain allocation of CSI-RS for row other",
                    fd_alloc_bit_location);
      return 2 * fd_alloc_bit_location;
    }
    default:
      report_fatal_error("Invalid CSI-RS row");
  }
}

static std::vector<csi_im_resource> make_csi_im_resources(const csi_builder_params& params)
{
  if (params.nof_ports > 4) {
    report_error("Unsupported number of antenna ports={}", params.nof_ports);
  }

  std::vector<csi_im_resource> res(1);

  // Make CSI-IM resource match in REs, symbols, slots with zp-CSI-RS>
  std::vector<zp_csi_rs_resource> zp_csi_rs_list = make_periodic_zp_csi_rs_resource_list(params);
  const zp_csi_rs_resource&       zp0            = zp_csi_rs_list[0];

  res[0].res_id = static_cast<csi_im_res_id_t>(0);
  res[0].csi_im_res_element_pattern.emplace();
  res[0].csi_im_res_element_pattern->pattern_type = csi_im_resource::csi_im_resource_element_pattern_type::pattern1;
  res[0].csi_im_res_element_pattern->subcarrier_location = get_subcarrier_location_from_fd_alloc_bit_location(
      zp0.res_mapping.fd_alloc.find_lowest(), zp0.res_mapping.fd_alloc.size());
  res[0].csi_im_res_element_pattern->symbol_location = zp0.res_mapping.first_ofdm_symbol_in_td;
  res[0].freq_band_rbs                               = zp0.res_mapping.freq_band_rbs;
  res[0].csi_res_period                              = *zp0.period;
  res[0].csi_res_offset                              = *zp0.offset;

  return res;
}

static std::vector<csi_im_resource_set> make_csi_im_resource_sets()
{
  std::vector<csi_im_resource_set> res_set(1);

  res_set[0].res_set_id        = static_cast<csi_im_res_set_id_t>(0);
  res_set[0].csi_ims_resources = {static_cast<csi_im_res_id_t>(0)};

  return res_set;
}

static std::vector<csi_resource_config> make_csi_resource_configs()
{
  std::vector<csi_resource_config> res_cfgs(3);

  // Resource 0 - Measurement.
  res_cfgs[0].res_cfg_id = static_cast<csi_res_config_id_t>(0);
  res_cfgs[0].csi_rs_res_set_list =
      csi_resource_config::nzp_csi_rs_ssb{.nzp_csi_rs_res_set_list = {static_cast<nzp_csi_rs_res_set_id_t>(0)}};
  res_cfgs[0].bwp_id   = to_bwp_id(0);
  res_cfgs[0].res_type = csi_resource_config::resource_type::periodic;

  // Resource 1 - Interference.
  res_cfgs[1]                     = res_cfgs[0];
  res_cfgs[1].res_cfg_id          = static_cast<csi_res_config_id_t>(1);
  res_cfgs[1].csi_rs_res_set_list = csi_resource_config::csi_im_resource_set_list{static_cast<csi_im_res_set_id_t>(0)};

  // Resource 2 - Tracking.
  res_cfgs[2]            = res_cfgs[0];
  res_cfgs[2].res_cfg_id = static_cast<csi_res_config_id_t>(2);
  res_cfgs[2].csi_rs_res_set_list =
      csi_resource_config::nzp_csi_rs_ssb{.nzp_csi_rs_res_set_list = {static_cast<nzp_csi_rs_res_set_id_t>(1)}};

  return res_cfgs;
}

static std::vector<csi_report_config> make_csi_report_configs(const csi_builder_params& params)
{
  std::vector<csi_report_config> reps(1);

  reps[0].report_cfg_id               = static_cast<csi_report_config_id_t>(0);
  reps[0].res_for_channel_meas        = static_cast<csi_res_config_id_t>(0);
  reps[0].csi_im_res_for_interference = static_cast<csi_res_config_id_t>(1);

  // Set Report Config.
  csi_report_config::periodic_or_semi_persistent_report_on_pucch report_cfg_type{};
  report_cfg_type.report_type = csi_report_config::periodic_or_semi_persistent_report_on_pucch::report_type_t::periodic;
  report_cfg_type.report_slot_period = convert_csi_resource_period_to_report_period(params.csi_rs_period);
  report_cfg_type.report_slot_offset = params.csi_report_slot_offset;
  report_cfg_type.pucch_csi_res_list = {
      csi_report_config::pucch_csi_resource{.ul_bwp = to_bwp_id(0), .pucch_res_id = pucch_res_id_t{9, 9}}};
  reps[0].report_cfg_type = report_cfg_type;

  reps[0].report_qty_type = csi_report_config::report_quantity_type_t::cri_ri_pmi_cqi;
  reps[0].non_pmi_port_indication.push_back(csi_report_config::port_index_for_8_ranks{
      .port_index_type = srsran::csi_report_config::port_index_for_8_ranks::port_index_type_t::port_index_1});

  reps[0].report_freq_cfg.emplace();
  reps[0].report_freq_cfg->cqi_format_ind = csi_report_config::cqi_format_indicator::wideband_cqi;
  reps[0].report_freq_cfg->pmi_format_ind = csi_report_config::pmi_format_indicator::wideband_pmi;

  if (params.nof_ports > 1) {
    reps[0].codebook_cfg.emplace();
    codebook_config::type1               type1{};
    codebook_config::type1::single_panel single_panel{};
    if (params.nof_ports == 2) {
      codebook_config::type1::single_panel::two_antenna_ports_two_tx_codebook_subset_restriction bitmap(6);
      bitmap.fill(0, 6, true);
      single_panel.nof_antenna_ports = bitmap;
    } else if (params.nof_ports == 4) {
      codebook_config::type1::single_panel::more_than_two_antenna_ports port_cfg{};
      port_cfg.n1_n2_restriction_type =
          codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::two_one;
      // Enable all beam combinations.
      port_cfg.n1_n2_restriction_value.resize(8);
      port_cfg.n1_n2_restriction_value.fill(0, 8, true);
      // Enable all i2 options.
      port_cfg.typei_single_panel_codebook_subset_restriction_i2.resize(16);
      port_cfg.typei_single_panel_codebook_subset_restriction_i2.fill(0, 16, true);
      single_panel.nof_antenna_ports = port_cfg;
    } else {
      report_error("Unsupported number of antenna ports {}", params.nof_ports);
    }

    // Maximum number of layers must be in line with available number of ports.
    srsran_assert(params.max_nof_layers <= params.nof_ports,
                  "Maximum number of layers cannot be greater than number of ports");

    // Limit the number of DL layers that can be requested by the UE via the Rank Indicator (RI).
    // As per TS 38.214, section 5.2.2.2.1, this can be done by setting the RI restriction bitmap to 0b11...11,
    // where the number of 1s is set to be equal to the number of desired layers.
    single_panel.typei_single_panel_ri_restriction.resize(8);
    single_panel.typei_single_panel_ri_restriction.from_uint64((1U << params.max_nof_layers) - 1U);
    type1.sub_type                      = single_panel;
    type1.codebook_mode                 = 1;
    reps[0].codebook_cfg->codebook_type = type1;
  }

  reps[0].is_group_based_beam_reporting_enabled = false;
  reps[0].cqi_table                             = cqi_table_t::table1;
  reps[0].subband_size                          = csi_report_config::subband_size_t::value1;

  return reps;
}

csi_meas_config srsran::csi_helper::make_csi_meas_config(const csi_builder_params& params)
{
  csi_meas_config csi_meas{};

  // NZP-CSI-RS-Resources.
  csi_meas.nzp_csi_rs_res_list = make_nzp_csi_rs_resource_list(params);

  // NZP-CSI-RS-ResourceSets.
  csi_meas.nzp_csi_rs_res_set_list = make_nzp_csi_rs_resource_sets();

  // csi-IM-Resources.
  csi_meas.csi_im_res_list = make_csi_im_resources(params);

  // csi-IM-ResourceSets.
  csi_meas.csi_im_res_set_list = make_csi_im_resource_sets();

  // CSI-ResourceConfig.
  csi_meas.csi_res_cfg_list = make_csi_resource_configs();

  // CSI-ReportConfig.
  csi_meas.csi_report_cfg_list = make_csi_report_configs(params);

  return csi_meas;
}
