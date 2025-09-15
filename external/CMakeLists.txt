#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_subdirectory(fmt)
add_subdirectory(uWebSockets)

set(SBOM_BACKWARD_VERSION "UNKNOWN" CACHE STRING "SBOM version for BACKWARD")
set(SBOM_CLI11_VERSION "2.4.2" CACHE STRING "SBOM version for CLI11")
set(SBOM_MPMCQUEUE_VERSION "UNKNOWN" CACHE STRING "SBOM version for MPMCQUEUE")
set(SBOM_SPSCQUEUE_VERSION "UNKNOWN" CACHE STRING "SBOM version for SPSCQUEUE")
set(SBOM_CONCURRENTQUEUE_VERSION "UNKNOWN" CACHE STRING "SBOM version for CONCURRENTQUEUE")
set(SBOM_EXPECTED_VERSION "1.1" CACHE STRING "SBOM version for EXPECTED")
set(SBOM_NLOHMANN_VERSION "3.12.0" CACHE STRING "SBOM version for NLOHMANN")
set(SBOM_SBOM_VERSION "1.3.0" CACHE STRING "SBOM version for SBOM")
