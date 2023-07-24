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

namespace srsran {

/// \brief NR operating bands in FR1 and FR2.
///
/// This enumeration abstracts the NR operating bands for FR1 and FR2 described in TS 38.104, Table 5.2-1 and
/// Table 5.2-2, respectively.
/// \remark: This is based on Rel. 17, version 17.8.0.
enum class nr_band {
  invalid = 0,
  // FR1 bands.
  n1   = 1,
  n2   = 2,
  n3   = 3,
  n5   = 5,
  n7   = 7,
  n8   = 8,
  n12  = 12,
  n13  = 13,
  n14  = 14,
  n18  = 18,
  n20  = 20,
  n24  = 24,
  n25  = 25,
  n26  = 26,
  n28  = 28,
  n29  = 29,
  n30  = 30,
  n34  = 34,
  n38  = 38,
  n39  = 39,
  n40  = 40,
  n41  = 41,
  n46  = 46,
  n48  = 48,
  n50  = 50,
  n51  = 51,
  n53  = 53,
  n65  = 65,
  n66  = 66,
  n67  = 67,
  n70  = 70,
  n71  = 71,
  n74  = 74,
  n75  = 75,
  n76  = 76,
  n77  = 77,
  n78  = 78,
  n79  = 79,
  n80  = 80,
  n81  = 81,
  n82  = 82,
  n83  = 83,
  n84  = 84,
  n85  = 85,
  n86  = 86,
  n89  = 89,
  n90  = 90,
  n91  = 91,
  n92  = 92,
  n93  = 93,
  n94  = 94,
  n95  = 95,
  n96  = 96,
  n97  = 97,
  n98  = 98,
  n99  = 99,
  n100 = 100,
  n101 = 101,
  n102 = 102,
  n104 = 104,
  // FR2 bands.
  n257 = 257,
  n258 = 258,
  n259 = 259,
  n260 = 260,
  n261 = 261,
  n262 = 262,
  n263 = 263
};

} // namespace srsran
