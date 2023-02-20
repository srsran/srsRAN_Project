/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/adt/span.h"

using namespace srsran;

sch_mcs_description srsran::pdsch_mcs_get_config(pdsch_mcs_table table_, sch_mcs_index index)
{
  // TS38.214 Table 5.1.3.1-1.
  static constexpr std::array<sch_mcs_description, 32> MCS_INDEX_TABLE_1 = {
      {{modulation_scheme::QPSK, 120.0F},  {modulation_scheme::QPSK, 157.0F},  {modulation_scheme::QPSK, 193.0F},
       {modulation_scheme::QPSK, 251.0F},  {modulation_scheme::QPSK, 308.0F},  {modulation_scheme::QPSK, 379.0F},
       {modulation_scheme::QPSK, 449.0F},  {modulation_scheme::QPSK, 526.0F},  {modulation_scheme::QPSK, 602.0F},
       {modulation_scheme::QPSK, 679.0F},  {modulation_scheme::QAM16, 340.0F}, {modulation_scheme::QAM16, 378.0F},
       {modulation_scheme::QAM16, 434.0F}, {modulation_scheme::QAM16, 490.0F}, {modulation_scheme::QAM16, 553.0F},
       {modulation_scheme::QAM16, 616.0F}, {modulation_scheme::QAM16, 658.0F}, {modulation_scheme::QAM64, 438.0F},
       {modulation_scheme::QAM64, 466.0F}, {modulation_scheme::QAM64, 517.0F}, {modulation_scheme::QAM64, 567.0F},
       {modulation_scheme::QAM64, 616.0F}, {modulation_scheme::QAM64, 666.0F}, {modulation_scheme::QAM64, 719.0F},
       {modulation_scheme::QAM64, 772.0F}, {modulation_scheme::QAM64, 822.0F}, {modulation_scheme::QAM64, 873.0F},
       {modulation_scheme::QAM64, 910.0F}, {modulation_scheme::QAM64, 948.0F}, {modulation_scheme::QPSK, 0.0F},
       {modulation_scheme::QAM16, 0.0F},   {modulation_scheme::QAM64, 0.0F}}};

  // TS38.214 Table 5.1.3.1-2.
  static constexpr std::array<sch_mcs_description, 32> MCS_INDEX_TABLE_2 = {
      {{modulation_scheme::QPSK, 120.0F},   {modulation_scheme::QPSK, 193.0F},   {modulation_scheme::QPSK, 308.0F},
       {modulation_scheme::QPSK, 449.0F},   {modulation_scheme::QPSK, 602.0F},   {modulation_scheme::QAM16, 378.0F},
       {modulation_scheme::QAM16, 434.0F},  {modulation_scheme::QAM16, 490.0F},  {modulation_scheme::QAM16, 553.0F},
       {modulation_scheme::QAM16, 616.0F},  {modulation_scheme::QAM16, 658.0F},  {modulation_scheme::QAM64, 466.0F},
       {modulation_scheme::QAM64, 517.0F},  {modulation_scheme::QAM64, 567.0F},  {modulation_scheme::QAM64, 616.0F},
       {modulation_scheme::QAM64, 666.0F},  {modulation_scheme::QAM64, 719.0F},  {modulation_scheme::QAM64, 772.0F},
       {modulation_scheme::QAM64, 822.0F},  {modulation_scheme::QAM64, 873.0F},  {modulation_scheme::QAM256, 682.5F},
       {modulation_scheme::QAM256, 711.0F}, {modulation_scheme::QAM256, 754.0F}, {modulation_scheme::QAM256, 797.0F},
       {modulation_scheme::QAM256, 841.0F}, {modulation_scheme::QAM256, 885.0F}, {modulation_scheme::QAM256, 916.5F},
       {modulation_scheme::QAM256, 948.0F}, {modulation_scheme::QPSK, 0.0F},     {modulation_scheme::QAM16, 0.0F},
       {modulation_scheme::QAM64, 0.0F},    {modulation_scheme::QAM256, 0.0F}}};

  // TS38.214 Table 5.1.3.1-3.
  static constexpr std::array<sch_mcs_description, 32> MCS_INDEX_TABLE_3 = {
      {{modulation_scheme::QPSK, 30.0F},   {modulation_scheme::QPSK, 40.0F},   {modulation_scheme::QPSK, 50.0F},
       {modulation_scheme::QPSK, 64.0F},   {modulation_scheme::QPSK, 78.0F},   {modulation_scheme::QPSK, 99.0F},
       {modulation_scheme::QPSK, 120.0F},  {modulation_scheme::QPSK, 157.0F},  {modulation_scheme::QPSK, 193.0F},
       {modulation_scheme::QPSK, 251.0F},  {modulation_scheme::QPSK, 308.0F},  {modulation_scheme::QPSK, 379.0F},
       {modulation_scheme::QPSK, 449.0F},  {modulation_scheme::QPSK, 526.0F},  {modulation_scheme::QPSK, 602.0F},
       {modulation_scheme::QAM16, 340.0F}, {modulation_scheme::QAM16, 378.0F}, {modulation_scheme::QAM16, 434.0F},
       {modulation_scheme::QAM16, 490.0F}, {modulation_scheme::QAM16, 553.0F}, {modulation_scheme::QAM16, 616.0F},
       {modulation_scheme::QAM64, 438.0F}, {modulation_scheme::QAM64, 466.0F}, {modulation_scheme::QAM64, 517.0F},
       {modulation_scheme::QAM64, 567.0F}, {modulation_scheme::QAM64, 616.0F}, {modulation_scheme::QAM64, 666.0F},
       {modulation_scheme::QAM64, 719.0F}, {modulation_scheme::QAM64, 772.0F}, {modulation_scheme::QPSK, 0.0F},
       {modulation_scheme::QAM16, 0.0F},   {modulation_scheme::QAM64, 0.0F}}};

  // Return the MCS configuration from the selected table.
  switch (table_) {
    case pdsch_mcs_table::qam64:
      return MCS_INDEX_TABLE_1[index.to_uint()];
    case pdsch_mcs_table::qam256:
      return MCS_INDEX_TABLE_2[index.to_uint()];
    case pdsch_mcs_table::qam64LowSe:
    default:
      return MCS_INDEX_TABLE_3[index.to_uint()];
  }
}