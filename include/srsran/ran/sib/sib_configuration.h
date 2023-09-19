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

namespace srsran {

/// \brief SIB retransmission periodicity in milliseconds as per TS38.331 Section 5.2.1.
/// \remark This is used for retransmission periodicity only; SIB1 periodicity is given as 160ms.
enum class sib1_rtx_periodicity { ms5 = 5, ms10 = 10, ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

/// \brief Converts the SIB1 periodicity property to its corresponding value in milliseconds.
inline unsigned sib1_rtx_periodicity_to_value(sib1_rtx_periodicity periodicity)
{
  return static_cast<unsigned>(periodicity);
}

} // namespace srsran
