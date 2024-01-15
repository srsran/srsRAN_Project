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

namespace srsran {

#ifdef BUILD_TYPE_RELEASE
static char build_mode[] = "Release";
#else
#ifdef BUILD_TYPE_DEBUG
static char build_mode[] = "Debug";
#else
#ifdef BUILD_TYPE_RELWITHDEBINFO
static char build_mode[] = "RelWithDebInfo";
#else
static char build_mode[] = "unknown";
#endif
#endif
#endif

static char build_hash[] = "@GIT_COMMIT_HASH@";

static char build_info[] = "commit @GIT_COMMIT_HASH@ on branch @GIT_BRANCH@";

} // namespace srsran