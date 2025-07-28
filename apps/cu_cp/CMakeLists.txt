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

add_executable(srscucp
        cu_cp.cpp
        cu_cp_appconfig_cli11_schema.cpp
)

install(TARGETS srscucp
        RUNTIME)

target_link_libraries(srscucp PRIVATE
        srsran_app_helpers
        srsran_app_services
        srsran_e1_gateway
        srsran_e2
        srsran_f1c_gateway
        srsran_ngap
        srsran_o_cu_cp_app_unit
        srsran_pcap
        srsran_support
        srsran_versioning
)

add_backward(srscucp)

target_include_directories(srscucp PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)
notify_binary_target(srscucp)
