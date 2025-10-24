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

#include "srsran/adt/tiny_optional.h"

int main()
{
  srsran::tiny_optional<std::unique_ptr<int>> a;
  srsran::tiny_optional<float>                b;
  srsran::tiny_optional<std::unique_ptr<int>> c = {};
  srsran::tiny_optional<float>                d;

  int a_val = 8;
  a         = &a_val;
  b         = 3.4;
  d         = {};
  abort();
}
