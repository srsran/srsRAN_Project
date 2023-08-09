/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/asn1/e2ap/e2sm.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"

class e2sm_kpm_meas_provider
{
public:
  virtual ~e2sm_kpm_meas_provider() = default;
  /// \brief Trigger collection of metric measurements
  /// \param[in] meas_type defines the metric to measure
  /// \param[in] label_info_list defines labels for the meas_type
  /// \param[in] ues if not empty, then measure meas_info_item for the listed UE IDs
  /// \param[in] cell_global_id if present measure meas_info_item within the cell scope
  /// \param[out] items contains measruement records (if ues is not empty, then one record for each UE)
  /// \return Returns True if measurement collection was successful
  virtual bool get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                             const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                             const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                             const srsran::optional<asn1::e2sm_kpm::cgi_c>    cell_global_id,
                             std::vector<asn1::e2sm_kpm::meas_record_item_c>& items) = 0;
};