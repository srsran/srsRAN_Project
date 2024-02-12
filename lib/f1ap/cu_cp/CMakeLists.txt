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


add_library(srsran_f1ap_cu
        f1ap_cu_impl.cpp
        f1ap_cu_factory.cpp
        procedures/f1_setup_procedure.cpp
        procedures/ue_context_setup_procedure.cpp
        procedures/ue_context_modification_procedure.cpp
        procedures/ue_context_release_procedure.cpp)
target_include_directories(srsran_f1ap_cu PRIVATE ..)
target_link_libraries(srsran_f1ap_cu srsran_support srsran_f1ap_common f1ap_asn1)
