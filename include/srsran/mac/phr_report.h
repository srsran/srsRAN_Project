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

#pragma once

#include "srsran/adt/interval.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"

namespace srsran {

/// \brief PH value expressed in dB interval as defined in Table 10.1.17.1-1 of TS 38.133.
using ph_db_range = interval<int>;

/// \brief P_CMAX,f,c value expressed in dBm interval as defined in Table 10.1.18.1-1 of TS 38.133.
/// \remark P_CMAX,f,c is the UE configured maximum output power used in computation of Power Headroom level. See
/// TS 38.321, 6.1.3.8 and TS 38.213, 7.1.1.
using p_cmax_dbm_range = interval<int>;

/// \brief Type of PH field reported by UE.
/// \remark See TS 38.321, 6.1.3.8 and 6.1.3.9.
enum class ph_field_type_t { type1, type2, type3 };

/// UL Power Headroom Report (PHR) of a cell.
struct cell_ph_report {
  /// Serving cell ID of the cell for which PH is reported. du_cell_index_t == 0 for SE-PHR.
  du_cell_index_t serv_cell_id;
  /// PH type. For SE-PHR its type1.
  ph_field_type_t ph_type;
  /// Indicates the power headroom level in dB interval.
  ph_db_range ph;
  /// UE configured maximum output power used in computation of Power Headroom level. This field maps to P_CMAX,f,c in
  /// table 6.1.3.8-2 of TS 38.321. This field is optional only in case of Multiple Entry PHR.
  optional<p_cmax_dbm_range> p_cmax;
};

/// UL Power Headroom Report (PHR).
struct phr_report {
  /// \brief Returns Single Entry PHR report.
  cell_ph_report get_se_phr() const
  {
    srsran_assert(not ph_reports.empty(), "Invalid access to SE-PHR report. Cause: No PHR report set.");
    return ph_reports.back();
  }

  /// \brief Sets Single-Entry PHR report.
  void set_se_phr(const cell_ph_report& cell_phr)
  {
    srsran_assert(cell_phr.serv_cell_id == to_du_cell_index(0), "Invalid serving cell id set in SE-PHR.");
    srsran_assert(cell_phr.ph_type == ph_field_type_t::type1, "Invalid PH type set in SE-PHR.");
    srsran_assert(cell_phr.p_cmax.has_value(), "P_CMAX must be set for SE-PHR.");
    ph_reports.clear();
    ph_reports.push_back(cell_phr);
  }

  /// \brief Returns whether the PHR is of type Single Entry PHR or not.
  /// \remark See TS 38.321, 6.1.3.8. A Single Entry PHR consists of a single PH report for PCell with Type1 PH field.
  bool is_se_phr() const
  {
    srsran_assert(ph_reports.size() > 0, "PHR report not set.");
    return ph_reports.size() == 1 and ph_reports.back().ph_type == ph_field_type_t::type1;
  }

  // TODO: Handle Multiple Entry PHR.

  /// \brief Returns PHR as list of PH report per cell.
  /// \remark Applies for both Single Entry PHR and multiple Entry PHR.
  span<const cell_ph_report> get_phr() const { return ph_reports; }

private:
  /// List containing PH reports. Only 1 report present in case of Single Entry PHR and can be more than 1 in case of
  /// Multiple Entry PHR.
  static_vector<cell_ph_report, MAX_NOF_DU_CELLS> ph_reports;
};

} // namespace srsran
