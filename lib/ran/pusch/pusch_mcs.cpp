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

#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/adt/span.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"

using namespace srsran;

sch_mcs_description srsran::pusch_mcs_get_config(pusch_mcs_table table,
                                                 sch_mcs_index   index,
                                                 bool            use_transform_precoder,
                                                 bool            tp_pi2bpsk_present)
{
  // TS38.214 Table 6.1.4.1-1.
  static constexpr std::array<sch_mcs_description, 32> MCS_INDEX_TABLE_1_TP = {
      {{modulation_scheme::PI_2_BPSK, 240.0F}, {modulation_scheme::PI_2_BPSK, 314.0F},
       {modulation_scheme::QPSK, 193.0F},      {modulation_scheme::QPSK, 251.0F},
       {modulation_scheme::QPSK, 308.0F},      {modulation_scheme::QPSK, 379.0F},
       {modulation_scheme::QPSK, 449.0F},      {modulation_scheme::QPSK, 526.0F},
       {modulation_scheme::QPSK, 602.0F},      {modulation_scheme::QPSK, 679.0F},
       {modulation_scheme::QAM16, 340.0F},     {modulation_scheme::QAM16, 378.0F},
       {modulation_scheme::QAM16, 434.0F},     {modulation_scheme::QAM16, 490.0F},
       {modulation_scheme::QAM16, 553.0F},     {modulation_scheme::QAM16, 616.0F},
       {modulation_scheme::QAM16, 658.0F},     {modulation_scheme::QAM64, 466.0F},
       {modulation_scheme::QAM64, 517.0F},     {modulation_scheme::QAM64, 567.0F},
       {modulation_scheme::QAM64, 616.0F},     {modulation_scheme::QAM64, 666.0F},
       {modulation_scheme::QAM64, 719.0F},     {modulation_scheme::QAM64, 772.0F},
       {modulation_scheme::QAM64, 822.0F},     {modulation_scheme::QAM64, 873.0F},
       {modulation_scheme::QAM64, 910.0F},     {modulation_scheme::QAM64, 948.0F},
       {modulation_scheme::PI_2_BPSK, 0.0F},   {modulation_scheme::QPSK, 0.0F},
       {modulation_scheme::QAM16, 0.0F},       {modulation_scheme::QAM64, 0.0F}}};

  // TS38.214 Table 6.1.4.1-2.
  static constexpr std::array<sch_mcs_description, 32> MCS_INDEX_TABLE_2_TP = {
      {{modulation_scheme::PI_2_BPSK, 60.0F},  {modulation_scheme::PI_2_BPSK, 80.0F},
       {modulation_scheme::PI_2_BPSK, 100.0F}, {modulation_scheme::PI_2_BPSK, 128.0F},
       {modulation_scheme::PI_2_BPSK, 156.0F}, {modulation_scheme::PI_2_BPSK, 198.0F},
       {modulation_scheme::QPSK, 120.0F},      {modulation_scheme::QPSK, 157.0F},
       {modulation_scheme::QPSK, 193.0F},      {modulation_scheme::QPSK, 251.0F},
       {modulation_scheme::QPSK, 308.0F},      {modulation_scheme::QPSK, 379.0F},
       {modulation_scheme::QPSK, 449.0F},      {modulation_scheme::QPSK, 526.0F},
       {modulation_scheme::QPSK, 602.0F},      {modulation_scheme::QPSK, 679.0F},
       {modulation_scheme::QAM16, 378.0F},     {modulation_scheme::QAM16, 434.0F},
       {modulation_scheme::QAM16, 490.0F},     {modulation_scheme::QAM16, 553.0F},
       {modulation_scheme::QAM16, 616.0F},     {modulation_scheme::QAM16, 658.0F},
       {modulation_scheme::QAM16, 699.0F},     {modulation_scheme::QAM16, 772.0F},
       {modulation_scheme::QAM64, 567.0F},     {modulation_scheme::QAM64, 616.0F},
       {modulation_scheme::QAM64, 666.0F},     {modulation_scheme::QAM64, 772.0F},
       {modulation_scheme::PI_2_BPSK, 0.0F},   {modulation_scheme::QPSK, 0.0F},
       {modulation_scheme::QAM16, 0.0F},       {modulation_scheme::QAM64, 0.0F}}};

  // Use PDSCH tables if no transform precoding is selected.
  if (!use_transform_precoder || (table == pusch_mcs_table::qam256)) {
    return pdsch_mcs_get_config(static_cast<pdsch_mcs_table>(table), index);
  }

  // Return the MCS configuration from the selected table.
  sch_mcs_description result = {};
  switch (table) {
    case pusch_mcs_table::qam64:
      result = MCS_INDEX_TABLE_1_TP[index.to_uint()];
      break;
    case pusch_mcs_table::qam64LowSe:
      result = MCS_INDEX_TABLE_2_TP[index.to_uint()];
      break;
    default:
      break;
  }

  if (result.modulation == modulation_scheme::PI_2_BPSK && !tp_pi2bpsk_present) {
    result.modulation = modulation_scheme::QPSK;
    result.target_code_rate /= 2;
  }

  return result;
}
