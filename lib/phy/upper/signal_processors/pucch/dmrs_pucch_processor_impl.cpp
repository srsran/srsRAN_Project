/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pucch_processor_format1_impl.h"
#include "dmrs_pucch_processor_format2_impl.h"

using namespace srsgnb;

using make_function = std::function<std::unique_ptr<dmrs_pucch_processor>(const low_papr_sequence_collection* c,
                                                                          const pucch_orthogonal_sequence*    occ)>;

static const std::array<make_function, static_cast<std::size_t>(pucch_format::NOF_FORMATS)> dmrs_pucch_factory = {
    // Format 0 doesn't use DM-RS
    nullptr,
    // Format 1
    [](const low_papr_sequence_collection* c, const pucch_orthogonal_sequence* occ) {
      return std::make_unique<dmrs_pucch_processor_format1_impl>(c, occ);
    },
    // Format 2
    [](const low_papr_sequence_collection* c, const pucch_orthogonal_sequence* occ) {
      return std::make_unique<dmrs_pucch_processor_format2_impl>();
    },
    // Formats 3 and 4 are not implemented
    nullptr,
    nullptr};

std::unique_ptr<dmrs_pucch_processor> srsgnb::create_dmrs_pucch_processor(const srsgnb::pucch_format&         format,
                                                                          const low_papr_sequence_collection* c,
                                                                          const pucch_orthogonal_sequence*    occ)
{
  srsran_assert(format < pucch_format::NOF_FORMATS, "Incorrect PUCCH format requested");
  auto make_fn = dmrs_pucch_factory[static_cast<std::size_t>(format)];

  srsran_assert(make_fn != nullptr, "Requested PUCCH Format not implemented");
  return make_fn(c, occ);
}