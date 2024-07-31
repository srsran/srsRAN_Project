#
# Copyright 2021-2024 Software Radio Systems Limited
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


add_library(srsran_e1ap_cu_up
        e1ap_cu_up_factory.cpp
        e1ap_cu_up_impl.cpp
        e1ap_cu_up_connection_handler.cpp
        procedures/e1ap_cu_up_setup_procedure.cpp
        procedures/bearer_context_modification_procedure.cpp
        procedures/bearer_context_release_procedure.cpp
        )
target_include_directories(srsran_e1ap_cu_up PRIVATE ..)
target_link_libraries(srsran_e1ap_cu_up e1ap_asn1 srsran_e1ap_common)
