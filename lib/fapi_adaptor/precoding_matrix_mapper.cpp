/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "precoding_matrix_mapper_functions.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace fapi_adaptor;

precoding_matrix_mapper::precoding_matrix_mapper(const precoding_matrix_mapper_codebook_offset_configuration& config) :
  ssb_codebook_offsets(config.ssb_codebook_offsets),
  pdsch_codebook_offsets(config.pdsch_codebook_offsets),
  pdcch_codebook_offsets(config.pdcch_codebook_offsets),
  csi_rs_codebook_offsets(config.csi_rs_codebook_offsets)
{
  srsran_assert(!ssb_codebook_offsets.empty(), "Invalid offset configuration");
  srsran_assert(!pdsch_codebook_offsets.empty(), "Invalid offset configuration");
  srsran_assert(!pdcch_codebook_offsets.empty(), "Invalid offset configuration");
  srsran_assert(!csi_rs_codebook_offsets.empty(), "Invalid offset configuration");
}

unsigned precoding_matrix_mapper::map(const mac_csi_rs_precoding_info& precoding_info) const
{
  return csi_rs_codebook_offsets[0] + get_csi_rs_precoding_matrix_index();
}

unsigned precoding_matrix_mapper::map(const mac_ssb_precoding_info& precoding_info) const
{
  return ssb_codebook_offsets[0] + get_ssb_precoding_matrix_index();
}

unsigned precoding_matrix_mapper::map(const mac_pdcch_precoding_info& precoding_info) const
{
  return pdcch_codebook_offsets[0] + get_pdcch_precoding_matrix_index();
}

/// Returns the precoding matrix index for the PDSCH codebook using the given offset and precoding information.
static unsigned get_pdsch_precoding_matrix_index(unsigned offset, const mac_pdsch_precoding_info& precoding_info)
{
  // 1 Port.
  if (!precoding_info.report) {
    return offset + get_pdsch_one_port_precoding_matrix_index();
  }

  // 2 Ports.
  if (variant_holds_alternative<csi_report_pmi::two_antenna_port>(precoding_info.report->type)) {
    return offset + get_pdsch_two_port_precoding_matrix_index(
                        variant_get<csi_report_pmi::two_antenna_port>(precoding_info.report->type).pmi);
  }

  return 0;
}

unsigned precoding_matrix_mapper::map(const mac_pdsch_precoding_info& precoding_info, unsigned nof_layers) const
{
  srsran_assert(nof_layers > 0, "Invalid number of layers={}", nof_layers);

  unsigned layer_index = nof_layers - 1U;
  srsran_assert(layer_index < pdsch_codebook_offsets.size(), "Invalid layer index value {}", layer_index);

  return get_pdsch_precoding_matrix_index(pdsch_codebook_offsets[layer_index], precoding_info);
}
