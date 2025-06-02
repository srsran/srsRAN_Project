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

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/ran/srs/srs_resource_configuration.h"
#include "srsran/scheduler/config/dmrs.h"

namespace srsran {

/// \brief Returns the PUSCH antenna ports mapping row index in TS 38.212, table 7.3.1.2.2-6 to 7.3.1.2.2-23.
/// \param[in] nof_layers         Number of layers.
/// \param[in] transform_precoder Set to \c true if the transform precoder is used.
/// \param[in] dmrs_cfg_type      PUSCH DM-RS configuration type.
/// \param[in] dmrs_max_len       PUSCH DM-RS maximum number of OFDM symbols.
/// \return The PUSCH antenna ports mapping row index.
unsigned get_pusch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                  bool             transform_precoder,
                                                  dmrs_config_type dmrs_cfg_type,
                                                  dmrs_max_length  dmrs_max_len);

/// \brief Returns the PUSCH precoding information mapping row index in TS 38.212, table 7.3.1.1.2-2/3/4/5.
/// \param[in] nof_layers Number of layers.
/// \param[in] max_rank   Maximum number of transmission layers.
/// \param[in] tpmi       PUSCH TPMI.
/// \return The PUSCH precoding information mapping row index.
unsigned get_pusch_precoding_info_row_index(unsigned                                      nof_layers,
                                            unsigned                                      max_rank,
                                            srs_resource_configuration::one_two_four_enum nof_srs_ports,
                                            unsigned                                      tpmi);

} // namespace srsran
