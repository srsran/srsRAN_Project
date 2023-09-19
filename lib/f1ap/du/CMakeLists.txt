#
# Copyright 2021-2023 Software Radio Systems Limited
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

add_subdirectory(procedures)
add_library(srsran_f1ap_du
        f1ap_du_impl.cpp
        f1ap_du_factory.cpp
        f1ap_du_connection_handler.cpp
        ue_context/f1ap_du_ue.cpp
        ue_context/ue_bearer_manager.cpp
        ue_context/f1c_du_bearer_impl.cpp
        ue_context/f1ap_du_ue_config_update.cpp)
target_include_directories(srsran_f1ap_du PRIVATE ..)
target_link_libraries(srsran_f1ap_du f1ap_asn1 srsran_f1ap_common srsran_f1ap_du_procedures srsran_f1u_du)
