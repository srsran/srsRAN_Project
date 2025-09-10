#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
