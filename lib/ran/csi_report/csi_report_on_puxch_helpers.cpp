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
#include "csi_report_on_puxch_helpers.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/csi_report/csi_report_on_puxch_utils.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

/// Gets the RI, LI, wideband CQI, and CRI fields bit-width as per TS38.212 Table 6.3.1.1.2-3.
static ri_li_cqi_cri_sizes get_ri_li_cqi_cri_sizes_typeI_single_panel(unsigned                 nof_csi_antenna_ports,
                                                                      ri_restriction_type      ri_restriction,
                                                                      csi_report_data::ri_type ri,
                                                                      unsigned                 nof_csi_rs_resources)
{
  ri_li_cqi_cri_sizes result;

  unsigned ri_uint              = ri.to_uint();
  unsigned ri_restriction_count = static_cast<unsigned>(ri_restriction.count());

  // Calculate RI field size.
  if (nof_csi_antenna_ports == 1) {
    result.ri = 0;
  } else {
    srsran_assert(ri_restriction.find_lowest(true) >= 0,
                  "The RI restriction field (i.e., {}) must have at least one true value.",
                  ri_restriction);

    if (nof_csi_antenna_ports == 2) {
      result.ri = std::min(1U, log2_ceil(ri_restriction_count));
    } else if (nof_csi_antenna_ports == 4) {
      result.ri = std::min(2U, log2_ceil(ri_restriction_count));
    } else {
      result.ri = log2_ceil(ri_restriction_count);
    }
  }

  // Calculate LI field size.
  if (nof_csi_antenna_ports == 1) {
    result.li = 0;
  } else if (nof_csi_antenna_ports == 2) {
    result.li = log2_ceil(ri_uint);
  } else {
    result.li = std::min(2U, log2_ceil(ri_uint));
  }

  // Wideband CQI for the first TB field size.
  result.wideband_cqi_first_tb = 4;

  // Wideband CQI for the second TB field size.
  if (ri.value() > 4) {
    result.wideband_cqi_second_tb = 4;
  } else {
    result.wideband_cqi_second_tb = 0;
  }

  // Subband differential CQI for the first TB.
  result.subband_diff_cqi_first_tb = 2;

  // Subband differential CQI for the second TB.
  if (ri.value() > 4) {
    result.subband_diff_cqi_second_tb = 2;
  } else {
    result.subband_diff_cqi_second_tb = 0;
  }

  // Calculate CRI field size. The number of CSI resources in the corresponding resource set must be at least one and up
  // to 64 (see TS38.331 Section 6.3.2, Information Element \c NZP-CSI-RS-ResourceSet).
  constexpr interval<unsigned, true> nof_csi_res_range(1, 64);
  srsran_assert(nof_csi_res_range.contains(nof_csi_rs_resources),
                "The number of CSI-RS resources in the resource set, i.e., {} exceeds the valid range {}.",
                nof_csi_rs_resources,
                nof_csi_res_range);
  result.cri = log2_ceil(nof_csi_rs_resources);

  return result;
}

ri_li_cqi_cri_sizes srsran::get_ri_li_cqi_cri_sizes(pmi_codebook_type        pmi_codebook,
                                                    ri_restriction_type      ri_restriction,
                                                    csi_report_data::ri_type ri,
                                                    unsigned                 nof_csi_rs_resources)
{
  unsigned nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(pmi_codebook);

  switch (pmi_codebook) {
    case pmi_codebook_type::one:
    case pmi_codebook_type::two:
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return get_ri_li_cqi_cri_sizes_typeI_single_panel(
          nof_csi_antenna_ports, ri_restriction, ri, nof_csi_rs_resources);
    case pmi_codebook_type::other:
    default:
      return {};
  }
}

/// Collects PMI sizes.
struct csi_report_typeI_single_panel_pmi_sizes {
  unsigned i_1_1;
  unsigned i_1_2;
  unsigned i_1_3;
  unsigned i_2;
};

/// Gets PMI sizes for TypeI-SinglePanel, Mode 1 codebook configuration as per TS38.212 Table 6.3.1.1.2-1.
static csi_report_typeI_single_panel_pmi_sizes
csi_report_get_pmi_sizes_typeI_single_panel_mode1(unsigned                 N1,
                                                  unsigned                 N2,
                                                  unsigned                 O1,
                                                  unsigned                 O2,
                                                  unsigned                 nof_csi_rs_antenna_ports,
                                                  csi_report_data::ri_type ri)
{
  if ((ri == 1) && (nof_csi_rs_antenna_ports > 2) && (N2 == 1)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 0, 2};
  }

  if ((ri == 2) && (nof_csi_rs_antenna_ports == 4) && (N2 == 1)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 1, 1};
  }

  if ((ri == 2) && (nof_csi_rs_antenna_ports > 4) && (N2 == 1)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 2, 1};
  }

  if (((ri == 3) || (ri == 4)) && (nof_csi_rs_antenna_ports == 4)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 0, 1};
  }

  report_error("Unhandled case with ri={} nof_csi_rs_antenna_ports={} N2={}.", ri, nof_csi_rs_antenna_ports, N2);
}

/// Gets PMI field bit-width for TypeI-SinglePanel and two port.
static unsigned csi_report_get_size_pmi_two_port(csi_report_data::ri_type ri)
{
  srsran_assert(ri <= 2, "Invalid rank indicator (i.e., {}).", ri);
  if (ri == 2) {
    return 1;
  }

  return 2;
}

/// Gets PMI field bit-width for TypeI-SinglePanel, four port and mode 1.
static unsigned csi_report_get_size_pmi_typeI_single_panel_4ports_mode1(unsigned nof_csi_rs_antenna_ports,
                                                                        csi_report_data::ri_type ri)
{
  unsigned count = 0;

  // Select values N1, N2, O1 and O2 from TS38.214 Table 5.2.2.2.1-2.
  unsigned N1 = 2;
  unsigned N2 = 1;
  unsigned O1 = 4;
  unsigned O2 = 1;

  csi_report_typeI_single_panel_pmi_sizes sizes =
      csi_report_get_pmi_sizes_typeI_single_panel_mode1(N1, N2, O1, O2, nof_csi_rs_antenna_ports, ri);

  count += sizes.i_1_1;
  count += sizes.i_1_2;
  count += sizes.i_1_3;
  count += sizes.i_2;

  return count;
}

unsigned srsran::csi_report_get_size_pmi(pmi_codebook_type codebook, csi_report_data::ri_type ri)
{
  unsigned nof_csi_rs_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(codebook);
  switch (codebook) {
    case pmi_codebook_type::two:
      return csi_report_get_size_pmi_two_port(ri);
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return csi_report_get_size_pmi_typeI_single_panel_4ports_mode1(nof_csi_rs_antenna_ports, ri);
    case pmi_codebook_type::one:
    case pmi_codebook_type::other:
    default:
      return 0;
  }
}

csi_report_data::wideband_cqi_type srsran::csi_report_unpack_wideband_cqi(csi_report_packed packed)
{
  srsran_assert(packed.size() == 4, "Packed size (i.e., {}) must be 4 bits.", packed.size());
  return packed.extract(0, 4);
}

/// Unpacks PMI for TypeI-SinglePort and two ports.
static csi_report_pmi csi_report_unpack_pmi_two_antenna_port(const csi_report_packed& packed)
{
  csi_report_pmi::two_antenna_port result;
  result.pmi = packed.extract(0, packed.size());

  return csi_report_pmi{result};
}

/// Unpacks PMI for TypeI-SinglePort, four ports and mode 1.
static csi_report_pmi csi_report_unpack_pmi_typeI_single_panel_4ports_mode1(const csi_report_packed& packed,
                                                                            unsigned nof_csi_rs_antenna_ports,
                                                                            csi_report_data::ri_type ri)
{
  unsigned                                        count = 0;
  csi_report_pmi::typeI_single_panel_4ports_mode1 result;

  // Select values N1, N2, O1 and O2 from TS38.214 Table 5.2.2.2.1-2;
  unsigned N1 = 2;
  unsigned N2 = 1;
  unsigned O1 = 4;
  unsigned O2 = 1;

  csi_report_typeI_single_panel_pmi_sizes sizes =
      csi_report_get_pmi_sizes_typeI_single_panel_mode1(N1, N2, O1, O2, nof_csi_rs_antenna_ports, ri);

  result.i_1_1 = packed.extract(count, sizes.i_1_1);
  count += sizes.i_1_1;

  srsran_assert(sizes.i_1_2 == 0, "PMI field i_1_2 size must be 0 bits for 4 ports.");

  if (ri > 1) {
    result.i_1_3.emplace(packed.extract(count, sizes.i_1_3));
    count += sizes.i_1_3;
  }

  result.i_2 = packed.extract(count, sizes.i_2);
  count += sizes.i_2;

  srsran_assert(packed.size() == count,
                "Packet input size (i.e., {}) does not match with the fields size (i.e., {})",
                packed.size(),
                count);

  return csi_report_pmi{result};
}

/// Unpacks PMI.
csi_report_pmi
srsran::csi_report_unpack_pmi(const csi_report_packed& packed, pmi_codebook_type codebook, csi_report_data::ri_type ri)
{
  unsigned nof_csi_rs_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(codebook);

  switch (codebook) {
    case pmi_codebook_type::two:
      return csi_report_unpack_pmi_two_antenna_port(packed);
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return csi_report_unpack_pmi_typeI_single_panel_4ports_mode1(packed, nof_csi_rs_antenna_ports, ri);
    case pmi_codebook_type::one:
    case pmi_codebook_type::other:
    default:
      return {};
  }
}

csi_report_data::ri_type srsran::csi_report_unpack_ri(const csi_report_packed&   ri_packed,
                                                      const ri_restriction_type& ri_restriction)
{
  unsigned ri = 1;
  if (!ri_packed.empty()) {
    ri = ri_packed.extract(0, ri_packed.size());

    srsran_assert(ri < ri_restriction.count(),
                  "Packed RI, i.e., {}, is out of bounds given the number of allowed rank values, i.e., {}.",
                  ri,
                  ri_restriction.count());

    ri = ri_restriction.get_bit_positions()[ri] + 1;
  }
  return ri;
}
