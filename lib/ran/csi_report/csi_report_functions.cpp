/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

static constexpr std::array<sch_mcs_description, 16> csi_report_wideband_cqi_table_1 = {{
    csi_report_reserved_wideband_cqi,
    {modulation_scheme::QPSK, 78},
    {modulation_scheme::QPSK, 120},
    {modulation_scheme::QPSK, 193},
    {modulation_scheme::QPSK, 308},
    {modulation_scheme::QPSK, 449},
    {modulation_scheme::QPSK, 602},
    {modulation_scheme::QAM16, 378},
    {modulation_scheme::QAM16, 490},
    {modulation_scheme::QAM16, 616},
    {modulation_scheme::QAM64, 466},
    {modulation_scheme::QAM64, 567},
    {modulation_scheme::QAM64, 666},
    {modulation_scheme::QAM64, 772},
    {modulation_scheme::QAM64, 873},
    {modulation_scheme::QAM64, 948},
}};

static constexpr std::array<sch_mcs_description, 16> csi_report_wideband_cqi_table_2 = {{
    csi_report_reserved_wideband_cqi,
    {modulation_scheme::QPSK, 78},
    {modulation_scheme::QPSK, 193},
    {modulation_scheme::QPSK, 449},
    {modulation_scheme::QAM16, 378},
    {modulation_scheme::QAM16, 490},
    {modulation_scheme::QAM16, 616},
    {modulation_scheme::QAM64, 466},
    {modulation_scheme::QAM64, 567},
    {modulation_scheme::QAM64, 666},
    {modulation_scheme::QAM64, 772},
    {modulation_scheme::QAM64, 873},
    {modulation_scheme::QAM256, 711},
    {modulation_scheme::QAM256, 797},
    {modulation_scheme::QAM256, 885},
    {modulation_scheme::QAM256, 948},
}};

static constexpr std::array<sch_mcs_description, 16> csi_report_wideband_cqi_table_3 = {{
    csi_report_reserved_wideband_cqi,
    {modulation_scheme::QPSK, 78},
    {modulation_scheme::QPSK, 193},
    {modulation_scheme::QPSK, 449},
    {modulation_scheme::QAM16, 378},
    {modulation_scheme::QAM16, 490},
    {modulation_scheme::QAM16, 616},
    {modulation_scheme::QAM64, 466},
    {modulation_scheme::QAM64, 567},
    {modulation_scheme::QAM64, 666},
    {modulation_scheme::QAM64, 772},
    {modulation_scheme::QAM64, 873},
    {modulation_scheme::QAM256, 711},
    {modulation_scheme::QAM256, 797},
    {modulation_scheme::QAM256, 885},
    {modulation_scheme::QAM256, 948},
}};

static sch_mcs_description csi_report_unpack_wideband_cqi(csi_report_cqi_table wideband_cqi_table,
                                                          csi_report_packed    packed)
{
  srsran_assert(packed.size() == 4, "Packed size (i.e., {}) must be 4 bits.", packed.size());
  unsigned i_wideband_cqi = packed.to_uint64();
  switch (wideband_cqi_table) {
    case csi_report_cqi_table::table_1:
      return csi_report_wideband_cqi_table_1[i_wideband_cqi];
    case csi_report_cqi_table::table_2:
      return csi_report_wideband_cqi_table_2[i_wideband_cqi];
    case csi_report_cqi_table::table_3:
    default:
      return csi_report_wideband_cqi_table_3[i_wideband_cqi];
  }
}

static unsigned csi_report_get_nof_csi_rs_antenna_ports(csi_report_pmi_codebook_configuration pmi_codebook)
{
  switch (pmi_codebook) {
    case csi_report_pmi_codebook_configuration::one:
      return 1;
    case csi_report_pmi_codebook_configuration::two:
      return 2;
    case csi_report_pmi_codebook_configuration::typeI_single_panel_4ports_mode1:
      return 4;
  }
  return 0;
}

static unsigned csi_report_get_cri_size(unsigned nof_csi_rs_resources)
{
  srsran_assert(nof_csi_rs_resources != 0, "The number of CSI-RS resources must be greater than 0.");
  return log2_ceil(nof_csi_rs_resources);
}

static unsigned csi_report_get_ri_size(unsigned nof_csi_antenna_ports, const csi_report_ri_restriction& ri_restriction)
{
  if (nof_csi_antenna_ports == 1) {
    return 0;
  }

  srsran_assert(ri_restriction.find_lowest(true) >= 0,
                "The RI restriction field (i.e., {}) must have at least one true value.",
                ri_restriction);

  if (nof_csi_antenna_ports == 2) {
    return std::min(1U, log2_ceil(static_cast<unsigned>(ri_restriction.count())));
  }

  return std::min(2U, log2_ceil(static_cast<unsigned>(ri_restriction.count())));
}

static unsigned csi_report_get_li_size(unsigned nof_csi_antenna_ports, csi_report_ri ri)
{
  if (nof_csi_antenna_ports == 1) {
    return 0;
  }

  if (nof_csi_antenna_ports == 2) {
    return log2_ceil(ri.to_uint());
  }

  return std::min(uint8_t{2}, log2_ceil(ri.to_uint()));
}

static unsigned csi_report_get_wideband_cqi_first_tb_size()
{
  return 4;
}

static unsigned csi_report_get_wideband_cqi_second_tb_size(csi_report_ri ri)
{
  if (ri <= 4) {
    return 0;
  }

  return 4;
}

struct csi_report_typeI_single_panel_pmi_sizes {
  unsigned i_1_1;
  unsigned i_1_2;
  unsigned i_1_3;
  unsigned i_2;
};

csi_report_typeI_single_panel_pmi_sizes
csi_report_get_pmi_sizes_typeI_single_panel_mode1(unsigned      N1,
                                                  unsigned      N2,
                                                  unsigned      O1,
                                                  unsigned      O2,
                                                  unsigned      nof_csi_rs_antenna_ports,
                                                  csi_report_ri ri)
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

  report_error("Unhandled case with  ri={} nof_csi_rs_antenna_ports={} N2={}.", ri, nof_csi_rs_antenna_ports, N2);
}

unsigned csi_report_get_size_pmi_one_port()
{
  return 0;
}

unsigned csi_report_get_size_pmi_two_port(csi_report_ri ri)
{
  srsran_assert(ri <= 2, "Invalid rank indicator (i.e., {}).", ri);
  if (ri == 2) {
    return 1;
  }

  return 2;
}

unsigned csi_report_get_size_pmi_typeI_single_panel_4ports_mode1(unsigned nof_csi_rs_antenna_ports, csi_report_ri ri)
{
  unsigned count = 0;

  // Select values N1, N2, O1 and O2 from TS38.214 Table 5.2.2.2.1-2;
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

unsigned csi_report_get_size_pmi(csi_report_pmi_codebook_configuration codebook,
                                 unsigned                              nof_csi_rs_antenna_ports,
                                 csi_report_ri                         ri)
{
  switch (codebook) {
    case csi_report_pmi_codebook_configuration::one:
      return csi_report_get_size_pmi_one_port();
    case csi_report_pmi_codebook_configuration::two:
      return csi_report_get_size_pmi_two_port(ri);
    case csi_report_pmi_codebook_configuration::typeI_single_panel_4ports_mode1:
      return csi_report_get_size_pmi_typeI_single_panel_4ports_mode1(nof_csi_rs_antenna_ports, ri);
  }
  return 0;
}

csi_report_pmi csi_report_unpack_pmi_one_antenna_port()
{
  csi_report_pmi result;
  result.type.emplace<csi_report_pmi::one_antenna_port>();
  return result;
}

csi_report_pmi csi_report_unpack_pmi_two_antenna_port(const csi_report_packed& packed)
{
  csi_report_pmi::two_antenna_port result;
  result.pmi = packed.to_uint64();

  return csi_report_pmi{result};
}

csi_report_pmi csi_report_unpack_pmi_typeI_single_panel_4ports_mode1(const csi_report_packed& packed,
                                                                     unsigned                 nof_csi_rs_antenna_ports,
                                                                     csi_report_ri            ri)
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

  if (sizes.i_1_1 > 0) {
    result.i_1_1 = packed.slice(count, count + sizes.i_1_1).to_uint64();
    count += sizes.i_1_1;
  }
  if (sizes.i_1_2 > 0) {
    result.i_1_2 = packed.slice(count, count + sizes.i_1_2).to_uint64();
    count += sizes.i_1_2;
  }
  if (sizes.i_1_3 > 0) {
    result.i_1_3 = packed.slice(count, count + sizes.i_1_3).to_uint64();
    count += sizes.i_1_3;
  }
  if (sizes.i_2 > 0) {
    result.i_2 = packed.slice(count, count + sizes.i_2).to_uint64();
    count += sizes.i_2;
  }

  srsran_assert(packed.size() == count,
                "Packet input size (i.e., {}) doe snot match with the fields size (i.e., {})",
                packed.size(),
                count);

  return csi_report_pmi{result};
}

static csi_report_pmi csi_report_unpack_pmi(const csi_report_packed&              packed,
                                            csi_report_pmi_codebook_configuration codebook,
                                            unsigned                              nof_csi_rs_antenna_ports,
                                            csi_report_ri                         ri)
{
  switch (codebook) {
    case csi_report_pmi_codebook_configuration::one:
      return csi_report_unpack_pmi_one_antenna_port();
    case csi_report_pmi_codebook_configuration::two:
      return csi_report_unpack_pmi_two_antenna_port(packed);
    case csi_report_pmi_codebook_configuration::typeI_single_panel_4ports_mode1:
      return csi_report_unpack_pmi_typeI_single_panel_4ports_mode1(packed, nof_csi_rs_antenna_ports, ri);
  }
  return {};
}

static units::bits get_csi_report_size_ri(const csi_report_configuration& config, csi_report_ri ri)
{
  // Counts number of bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned count                 = 0;
  unsigned nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  // CRI.
  if (config.enable_cri) {
    count += csi_report_get_cri_size(config.nof_csi_rs_resources);
  }

  // RI.
  if (config.enable_ri) {
    count += csi_report_get_ri_size(nof_csi_antenna_ports, config.ri_restriction);
  }

  // LI.
  if (config.enable_li) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for LI reports.");
    count += csi_report_get_li_size(nof_csi_antenna_ports, ri);
  }

  // Ignore padding.

  // PMI.
  if (config.enable_pmi) {
    count += csi_report_get_size_pmi(config.pmi_codebook, nof_csi_antenna_ports, ri);
  }

  // Wideband CQI for the first TB.
  if (config.enable_wideband_cqi) {
    count += csi_report_get_wideband_cqi_first_tb_size();
  }

  // Wideband CQI for the second TB.
  if (config.enable_wideband_cqi) {
    count += csi_report_get_wideband_cqi_second_tb_size(ri);
  }

  return units::bits{count};
}

units::bits srsran::get_csi_report_size(const csi_report_configuration& config)
{
  units::bits csi_report_size(0);
  unsigned    nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  for (unsigned ri = 1, ri_end = nof_csi_antenna_ports + 1; ri != ri_end; ++ri) {
    csi_report_size = std::max(csi_report_size, get_csi_report_size_ri(config, ri));
  }

  return csi_report_size;
}

csi_report_data srsran::csi_report_unpack(const csi_report_packed& packed, const csi_report_configuration& config)
{
  // Validate input size.
  units::bits csi_report_size = get_csi_report_size(config);
  srsran_assert(csi_report_size <= csi_report_max_size,
                "The report size (i.e., {}) exceeds the maximum report size (i.e., {})",
                csi_report_size,
                csi_report_max_size);
  srsran_assert(packed.size() == csi_report_size.value(),
                "The number of packed bits (i.e., {}) is not equal to the CSI report size (i.e., {})",
                units::bits(packed.size()),
                csi_report_size);

  unsigned nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  // Unpacks bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned        count = 0;
  csi_report_data data;

  // CRI.
  if (config.enable_cri) {
    unsigned field_size = csi_report_get_cri_size(config.nof_csi_rs_resources);
    if (field_size != 0) {
      data.cri.emplace(packed.slice(count, count + field_size).to_uint64());
      count += field_size;
    } else {
      data.cri.emplace(0);
    }
  }

  // RI.
  if (config.enable_ri) {
    unsigned field_size = csi_report_get_ri_size(nof_csi_antenna_ports, config.ri_restriction);
    if (field_size != 0) {
      data.ri.emplace(packed.slice(count, count + field_size).to_uint64() + 1);
      count += field_size;
    } else {
      data.ri.emplace(1);
    }
  }

  // LI.
  if (config.enable_li) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for LI reports.");
    unsigned field_size = csi_report_get_li_size(nof_csi_antenna_ports, data.ri.value());
    if (field_size != 0) {
      data.li.emplace(packed.slice(count, count + field_size).to_uint64());
      count += field_size;
    } else {
      data.li.emplace(1);
    }
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
    unsigned field_size = csi_report_get_size_pmi(config.pmi_codebook, nof_csi_antenna_ports, data.ri.value());
    if (field_size != 0) {
      csi_report_unpack_pmi(
          packed.slice(count, count + field_size), config.pmi_codebook, nof_csi_antenna_ports, data.ri.value());
      count += field_size;
    }
  }

  // Wideband CQI for the first TB.
  if (config.enable_wideband_cqi) {
    unsigned field_size = csi_report_get_wideband_cqi_first_tb_size();
    if (field_size != 0) {
      data.first_tb_wideband_cqi.emplace(
          csi_report_unpack_wideband_cqi(config.wideband_cqi_table, packed.slice(count, count + field_size)));
      count += field_size;
    }
  }

  // Wideband CQI for the second TB.
  if (config.enable_wideband_cqi) {
    srsran_assert(config.enable_ri, "Enabling RI is necessary for second TB wideband CQI reports.");
    unsigned field_size = csi_report_get_wideband_cqi_second_tb_size(data.ri.value());
    if (field_size != 0) {
      data.second_wideband_cqi.emplace(
          csi_report_unpack_wideband_cqi(config.wideband_cqi_table, packed.slice(count, count + field_size)));
      count += field_size;
    }
  }

  srsran_assert(count == csi_report_size.value(),
                "The number of read bits (i.e., {}) is not equal to the CSI report size (i.e., {})",
                units::bits(count),
                csi_report_size);

  return data;
}