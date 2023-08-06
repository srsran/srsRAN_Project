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

set(SOURCES
        ngap_factory.cpp
        ngap_impl.cpp
        ngap_asn1_packer.cpp
        procedures/ng_setup_procedure.cpp
        procedures/ngap_initial_context_setup_procedure.cpp
        procedures/ngap_pdu_session_resource_setup_procedure.cpp
        procedures/ngap_pdu_session_resource_modify_procedure.cpp
        procedures/ngap_pdu_session_resource_release_procedure.cpp
        procedures/ngap_handover_preparation_procedure.cpp
        procedures/ngap_handover_resource_allocation_procedure.cpp
    )

add_library(srsran_ngap STATIC ${SOURCES})
target_link_libraries(srsran_ngap ngap_asn1 srsran_cu_cp srsran_support)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
