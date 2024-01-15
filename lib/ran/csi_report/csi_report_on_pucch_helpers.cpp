/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "csi_report_on_puxch_helpers.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_on_puxch_utils.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

static units::bits get_csi_report_pucch_size_cri_ri_li_pmi_cqi(const csi_report_configuration& config,
                                                               csi_report_data::ri_type        ri)
{
  // Counts number of bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned count = 0;

  // Gets RI, LI, CQI and CRI field sizes.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

  // CRI.
  count += sizes.cri;

  // RI.
  count += sizes.ri;

  // LI.
  if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    count += sizes.li;
  }

  // Ignore padding.

  // PMI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    count += csi_report_get_size_pmi(config.pmi_codebook, ri);
  }

  // Wideband CQI ...
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    // For the first TB.
    count += sizes.wideband_cqi_first_tb;

    // For the second TB.
    count += sizes.wideband_cqi_second_tb;
  }

  return units::bits{count};
}

static csi_report_data csi_report_unpack_pucch_cri_ri_li_pmi_cqi(const csi_report_packed&        packed,
                                                                 const csi_report_configuration& config)
{
  csi_report_data data;

  // Validate input size.
  units::bits csi_report_size = get_csi_report_pucch_size(config);
  srsran_assert(csi_report_size <= csi_report_max_size,
                "The report size (i.e., {}) exceeds the maximum report size (i.e., {}).",
                csi_report_size,
                csi_report_max_size);

  // Verify the CSI payload size.
  srsran_assert(packed.size() == csi_report_size.value(),
                "The number of packed bits (i.e., {}) is not equal to the CSI report size (i.e., {}).",
                units::bits(packed.size()),
                csi_report_size);

  // Gets RI, LI, CQI and CRI field sizes as the rank was one.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);

  // Unpacks bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned count = 0;

  // CRI.
  unsigned cri = 0;
  if (sizes.cri > 0) {
    cri = packed.extract(count, sizes.cri);
  }
  data.cri.emplace(cri);
  count += sizes.cri;

  // RI.
  csi_report_data::ri_type ri = csi_report_unpack_ri(packed.slice(count, count + sizes.ri), config.ri_restriction);
  data.ri.emplace(ri.to_uint());
  count += sizes.ri;

  // LI.
  if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    // Gets RI, LI, CQI and CRI field sizes.
    ri_li_cqi_cri_sizes sizes_ri =
        get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

    unsigned li = 0;
    if (sizes_ri.li != 0) {
      li = packed.extract(count, sizes_ri.li);
    }

    data.li.emplace(li);
    count += sizes_ri.li;
  }

  // Padding.
  units::bits csi_report_size_ri = get_csi_report_pucch_size_cri_ri_li_pmi_cqi(config, ri);
  srsran_assert(
      csi_report_size >= csi_report_size_ri,
      "The report size (i.e., {}) must be equal to or greater than the report size without padding (i.e., {}).",
      csi_report_size,
      csi_report_size_ri);
  count += csi_report_size.value() - csi_report_size_ri.value();

  // PMI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    unsigned pmi_size = csi_report_get_size_pmi(config.pmi_codebook, ri);

    if (pmi_size != 0) {
      data.pmi.emplace(csi_report_unpack_pmi(packed.slice(count, count + pmi_size), config.pmi_codebook, ri));
      count += pmi_size;
    }
  }

  // Wideband CQI ...
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    // For the first TB.
    data.first_tb_wideband_cqi.emplace(
        csi_report_unpack_wideband_cqi(packed.slice(count, count + sizes.wideband_cqi_first_tb)));
    count += sizes.wideband_cqi_first_tb;

    // For the second TB.
    if (sizes.wideband_cqi_second_tb != 0) {
      data.second_tb_wideband_cqi.emplace(
          csi_report_unpack_wideband_cqi(packed.slice(count, count + sizes.wideband_cqi_second_tb)));
      count += sizes.wideband_cqi_second_tb;
    }
  }

  srsran_assert(count == csi_report_size.value(),
                "The number of read bits (i.e., {}) is not equal to the CSI report size (i.e., {})",
                units::bits(count),
                csi_report_size);

  return data;
}

/// Gets the CSI report size in function of the RI.
static units::bits get_csi_report_size_ri(const csi_report_configuration& config, csi_report_data::ri_type ri)
{
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return get_csi_report_pucch_size_cri_ri_li_pmi_cqi(config, ri);
    case csi_report_quantities::other:
    default:
      return units::bits{0};
  }
}

units::bits srsran::get_csi_report_pucch_size(const csi_report_configuration& config)
{
  units::bits csi_report_size(0);
  unsigned    nof_csi_antenna_ports = csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook);

  // For each possible RI, find the largest CSI report size.
  for (unsigned ri = 1, ri_end = nof_csi_antenna_ports + 1; ri != ri_end; ++ri) {
    csi_report_size = std::max(csi_report_size, get_csi_report_size_ri(config, ri));
  }

  return csi_report_size;
}

bool srsran::validate_pucch_csi_payload(const csi_report_packed& packed, const csi_report_configuration& config)
{
  // CSI report configuration is invalid.
  if (!is_valid(config)) {
    return false;
  }

  // The number of packed bits does not match the expected CSI report size.
  if (packed.size() != get_csi_report_pucch_size(config).value()) {
    return false;
  }

  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);
  unsigned ri_packed = packed.extract(sizes.cri, sizes.ri);

  // The RI is out of bounds given the number of allowed rank values.
  if (ri_packed >= config.ri_restriction.count()) {
    return false;
  }

  return true;
}

csi_report_data srsran::csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config)
{
  srsran_assert(config.pmi_codebook != pmi_codebook_type::other, "Unsupported PMI codebook type.");

  srsran_assert((config.pmi_codebook == pmi_codebook_type::one) ||
                    (config.ri_restriction.size() >= csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook)),
                "The RI restriction set size, i.e., {}, is smaller than the number of CSI-RS ports, i.e., {}.",
                config.ri_restriction.size(),
                csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook));

  srsran_assert(
      (config.pmi_codebook == pmi_codebook_type::one) ||
          (config.ri_restriction.find_highest() <
           static_cast<int>(csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook))),
      "The RI restriction set, i.e., {}, allows higher rank values than the number of CSI-RS ports, i.e., {}.",
      config.ri_restriction,
      csi_report_get_nof_csi_rs_antenna_ports(config.pmi_codebook));

  // Select unpacking depending on the CSI report quantities.
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return csi_report_unpack_pucch_cri_ri_li_pmi_cqi(packed, config);
    case csi_report_quantities::other:
    default:
      report_error("Invalid CSI report quantities.");
  }
}
