/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_unpacking.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

static csi_report_wideband_cqi_type csi_report_unpack_wideband_cqi(csi_report_packed packed)
{
  srsran_assert(packed.size() == 4, "Packed size (i.e., {}) must be 4 bits.", packed.size());
  return packed.extract(0, 4);
}

static unsigned csi_report_get_nof_csi_rs_antenna_ports(pmi_codebook_type pmi_codebook)
{
  switch (pmi_codebook) {
    case pmi_codebook_type::one:
      return 1;
    case pmi_codebook_type::two:
      return 2;
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return 4;
    case pmi_codebook_type::other:
    default:
      return 0;
  }
}

struct ri_li_cqi_cri_sizes {
  unsigned ri;
  unsigned li;
  unsigned cqi_first_tb;
  unsigned cqi_second_tb;
  unsigned subband_diff_cqi_first_tb;
  unsigned subband_diff_cqi_second_tb;
  unsigned cri;
};

ri_li_cqi_cri_sizes get_ri_li_cqi_cri_sizes_typeI_single_panel(unsigned                 nof_csi_antenna_ports,
                                                               ri_restriction_type      ri_restriction,
                                                               csi_report_data::ri_type ri,
                                                               unsigned                 nof_csi_rs_resources)
{
  ri_li_cqi_cri_sizes result;

  // Calculate RI field size.
  if (nof_csi_antenna_ports == 1) {
    result.ri = 0;
  } else {
    srsran_assert(ri_restriction.find_lowest(true) >= 0,
                  "The RI restriction field (i.e., {}) must have at least one true value.",
                  ri_restriction);

    if (nof_csi_antenna_ports == 2) {
      result.ri = std::min(1U, log2_ceil(static_cast<unsigned>(ri_restriction.count())));
    } else if (nof_csi_antenna_ports == 4) {
      result.ri = std::min(2U, log2_ceil(static_cast<unsigned>(ri_restriction.count())));
    } else {
      result.ri = log2_ceil(static_cast<unsigned>(ri_restriction.count()));
    }
  }

  // Calculate LI field size.
  if (nof_csi_antenna_ports == 1) {
    result.li = 0;
  } else if (nof_csi_antenna_ports == 2) {
    result.li = log2_ceil(ri.to_uint());
  } else {
    result.li = std::min(uint8_t{2}, log2_ceil(ri.to_uint()));
  }

  // Wideband CQI for the first TB field size.
  result.cqi_first_tb = 4;

  // Wideband CQI for the second TB field size.
  if (ri.value() > 4) {
    result.cqi_second_tb = 4;
  } else {
    result.cqi_second_tb = 0;
  }

  // Subband differential CQI for the first TB.
  result.subband_diff_cqi_first_tb = 2;

  // Subband differential CQI for the second TB.
  if (ri.value() > 4) {
    result.subband_diff_cqi_second_tb = 2;
  } else {
    result.subband_diff_cqi_second_tb = 0;
  }

  // Calculate CRI field size.
  srsran_assert(nof_csi_rs_resources != 0, "The number of CSI-RS resources must be greater than 0.");
  result.cri = log2_ceil(nof_csi_rs_resources);

  return result;
}

ri_li_cqi_cri_sizes get_ri_li_cqi_cri_sizes(pmi_codebook_type        pmi_codebook,
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

struct csi_report_typeI_single_panel_pmi_sizes {
  unsigned i_1_1;
  unsigned i_1_2;
  unsigned i_1_3;
  unsigned i_2;
};

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

static unsigned csi_report_get_size_pmi_one_port()
{
  return 0;
}

static unsigned csi_report_get_size_pmi_two_port(csi_report_data::ri_type ri)
{
  srsran_assert(ri <= 2, "Invalid rank indicator (i.e., {}).", ri);
  if (ri == 2) {
    return 1;
  }

  return 2;
}

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

static unsigned csi_report_get_size_pmi(pmi_codebook_type codebook,

                                        csi_report_data::ri_type ri)
{
  unsigned nof_csi_rs_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(codebook);
  switch (codebook) {
    case pmi_codebook_type::one:
      return csi_report_get_size_pmi_one_port();
    case pmi_codebook_type::two:
      return csi_report_get_size_pmi_two_port(ri);
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return csi_report_get_size_pmi_typeI_single_panel_4ports_mode1(nof_csi_rs_antenna_ports, ri);
    case pmi_codebook_type::other:
    default:
      return 0;
  }
}

static csi_report_pmi csi_report_unpack_pmi_one_antenna_port()
{
  csi_report_pmi result;
  result.type.emplace<csi_report_pmi::one_antenna_port>();
  return result;
}

static csi_report_pmi csi_report_unpack_pmi_two_antenna_port(const csi_report_packed& packed)
{
  csi_report_pmi::two_antenna_port result;
  result.pmi = packed.extract(0, packed.size());

  return csi_report_pmi{result};
}

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

  result.i_1_2 = packed.extract(count, sizes.i_1_2);
  count += sizes.i_1_2;

  if (ri > 1) {
    result.i_1_3.emplace(packed.extract(count, sizes.i_1_3));
    count += sizes.i_1_3;
  }

  result.i_2 = packed.extract(count, sizes.i_2);
  count += sizes.i_2;

  srsran_assert(packed.size() == count,
                "Packet input size (i.e., {}) doe snot match with the fields size (i.e., {})",
                packed.size(),
                count);

  return csi_report_pmi{result};
}

static csi_report_pmi
csi_report_unpack_pmi(const csi_report_packed& packed, pmi_codebook_type codebook, csi_report_data::ri_type ri)
{
  unsigned nof_csi_rs_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(codebook);

  switch (codebook) {
    case pmi_codebook_type::one:
      return csi_report_unpack_pmi_one_antenna_port();
    case pmi_codebook_type::two:
      return csi_report_unpack_pmi_two_antenna_port(packed);
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return csi_report_unpack_pmi_typeI_single_panel_4ports_mode1(packed, nof_csi_rs_antenna_ports, ri);
    case pmi_codebook_type::other:
    default:
      return {};
  }
}

static units::bits get_csi_report_size_ri(const csi_report_configuration& config, csi_report_data::ri_type ri)
{
  // Counts number of bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned count = 0;

  // Gets RI, LI, CQI and CRI field sizes.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

  // CRI.
  if (config.enable_cri) {
    count += sizes.cri;
  }

  // RI.
  if (config.enable_ri) {
    count += sizes.ri;
  }

  // LI.
  if (config.enable_li) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for LI reports.");
    count += sizes.li;
  }

  // Ignore padding.

  // PMI.
  if (config.enable_pmi) {
    count += csi_report_get_size_pmi(config.pmi_codebook, ri);
  }

  // Wideband CQI for the first TB.
  if (config.enable_wideband_cqi) {
    count += sizes.cqi_first_tb;
  }

  // Wideband CQI for the second TB.
  if (config.enable_wideband_cqi) {
    count += sizes.cqi_second_tb;
  }

  return units::bits{count};
}

units::bits srsran::get_csi_report_pucch_size(const csi_report_configuration& config)
{
  units::bits csi_report_size(0);
  unsigned    nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  for (unsigned ri = 1, ri_end = nof_csi_antenna_ports + 1; ri != ri_end; ++ri) {
    csi_report_size = std::max(csi_report_size, get_csi_report_size_ri(config, ri));
  }

  return csi_report_size;
}

csi_report_data srsran::csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config)
{
  // Validate input size.
  units::bits csi_report_size = get_csi_report_pucch_size(config);
  srsran_assert(csi_report_size <= csi_report_max_size,
                "The report size (i.e., {}) exceeds the maximum report size (i.e., {})",
                csi_report_size,
                csi_report_max_size);
  srsran_assert(packed.size() == csi_report_size.value(),
                "The number of packed bits (i.e., {}) is not equal to the CSI report size (i.e., {})",
                units::bits(packed.size()),
                csi_report_size);

  // Gets RI, LI, CQI and CRI field sizes as the rank was one.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);

  // Unpacks bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned        count = 0;
  csi_report_data data;

  // CRI.
  if (config.enable_cri) {
    unsigned cri = 0;
    if (sizes.cri > 0) {
      cri = packed.extract(count, sizes.cri);
    }
    data.cri.emplace(cri);
    count += sizes.cri;
  }

  // RI.
  if (config.enable_ri) {
    unsigned ri = 1;
    if (sizes.ri > 0) {
      ri = packed.extract(count, sizes.ri) + 1;
    }
    data.ri.emplace(ri);
    count += sizes.ri;
  }

  // LI.
  if (config.enable_li) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for LI reports.");
    // Gets RI, LI, CQI and CRI field sizes.
    ri_li_cqi_cri_sizes sizes_ri = get_ri_li_cqi_cri_sizes(
        config.pmi_codebook, config.ri_restriction, data.ri.value(), config.nof_csi_rs_resources);

    unsigned li = 0;
    if (sizes_ri.li != 0) {
      li = packed.extract(count, sizes_ri.li);
    }

    data.li.emplace(li);
    count += sizes_ri.li;
  }

  // Padding.
  units::bits csi_report_size_ri = get_csi_report_size_ri(config, data.ri.value());
  srsran_assert(
      csi_report_size >= csi_report_size_ri,
      "The report size (i.e., {}) must be equal to or greater than the report size without padding (i.e., {}).",
      csi_report_size,
      csi_report_size_ri);
  count += csi_report_size.value() - csi_report_size_ri.value();

  // PMI.
  if (config.enable_pmi) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for PMI reports.");
    unsigned pmi_size = csi_report_get_size_pmi(config.pmi_codebook, data.ri.value());

    data.pmi.emplace(
        csi_report_unpack_pmi(packed.slice(count, count + pmi_size), config.pmi_codebook, data.ri.value()));
    count += pmi_size;
  }

  // Wideband CQI for the first TB.
  if (config.enable_wideband_cqi) {
    data.first_tb_wideband_cqi.emplace(csi_report_unpack_wideband_cqi(packed.slice(count, count + sizes.cqi_first_tb)));
    count += sizes.cqi_first_tb;
  }

  // Wideband CQI for the second TB.
  if (config.enable_wideband_cqi) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for second TB wideband CQI reports.");
    if (sizes.cqi_second_tb != 0) {
      data.second_tb_wideband_cqi.emplace(
          csi_report_unpack_wideband_cqi(packed.slice(count, count + sizes.cqi_second_tb)));
      count += sizes.cqi_second_tb;
    }
  }

  srsran_assert(count == csi_report_size.value(),
                "The number of read bits (i.e., {}) is not equal to the CSI report size (i.e., {})",
                units::bits(count),
                csi_report_size);

  return data;
}