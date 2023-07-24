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

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/mac/phr_report.h"

namespace srsran {

/// \brief Converts PH value reported by UE to dB interval based on mapping defined in Table 10.1.17.1-1 of TS 38.133.
/// \param[in] ph PH value reported by UE. This field maps to PH in table 6.1.3.8-1 of TS 38.321.
/// \return Returns the dB interval to which reported PH maps to.
inline ph_db_range ph_to_db_range(unsigned ph)
{
  static const int ph_min_db_value = -32;
  static const int ph_max_db_value = 38;

  if (ph == 0) {
    // PH is < -32dB.
    return {ph_min_db_value, ph_min_db_value};
  }
  if (ph == 63) {
    // PH is >= 38dB.
    return {ph_max_db_value, ph_max_db_value};
  }
  if (ph < 55) {
    return {ph_min_db_value + ph - 1, ph_min_db_value + ph};
  }
  return {ph_min_db_value + ph - 1, ph_min_db_value + ph + 1};
}

/// \brief Converts P_CMAX,f,c value reported by UE to dBm interval based on mapping defined in Table 10.1.18.1-1 of
/// TS 38.133.
/// \param[in] p_cmax P_CMAX,f,c value reported by UE. This field maps to PH in table 6.1.3.8-2 of TS 38.321.
/// \return Returns the dBm interval to which reported P_CMAX,f,c maps to.
inline p_cmax_dbm_range p_cmax_to_dbm_range(unsigned p_cmax)
{
  static const int p_cmax_min_db_value = -29;
  static const int p_cmax_max_db_value = 33;

  if (p_cmax == 0) {
    // P_CMAX,f,c is < -29dBm.
    return {p_cmax_min_db_value, p_cmax_min_db_value};
  }
  if (p_cmax == 63) {
    // P_CMAX,f,c is >= 33dBm.
    return {p_cmax_max_db_value, p_cmax_max_db_value};
  }
  return {p_cmax_min_db_value + p_cmax - 1, p_cmax_min_db_value + p_cmax};
}

/// \brief Decode Single Entry PHR.
/// \remark See TS 38.321, 6.1.3.8.
inline phr_report decode_se_phr(byte_buffer_view payload)
{
  srsran_sanity_check(not payload.empty(), "Trying to decode SE-PHR but payload is empty.");
  srsran_sanity_check(payload.length() == 2, "Invalid payload length={} while decoding SE-PHR.", payload.length());
  phr_report se_phr = {};
  se_phr.set_se_phr(cell_ph_report{.serv_cell_id = to_du_cell_index(0),
                                   .ph_type      = ph_field_type_t::type1,
                                   .ph           = ph_to_db_range(payload[0] & 0b00111111U),
                                   .p_cmax       = p_cmax_to_dbm_range(payload[1] & 0b00111111U)});
  return se_phr;
}

} // namespace srsran
