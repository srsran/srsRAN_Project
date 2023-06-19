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
#include "srsran/ran/csi_report/csi_report_wideband_cqi.h"

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

sch_mcs_description srsran::to_mcs_description(cqi_table_type table, csi_report_wideband_cqi_type wideband_cqi)
{
  unsigned i_wideband_cqi = wideband_cqi.to_uint();
  switch (table) {
    case cqi_table_type::table_1:
      return csi_report_wideband_cqi_table_1[i_wideband_cqi];
    case cqi_table_type::table_2:
      return csi_report_wideband_cqi_table_2[i_wideband_cqi];
    case cqi_table_type::table_3:
    default:
      return csi_report_wideband_cqi_table_3[i_wideband_cqi];
  }
}
