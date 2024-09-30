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

add_subdirectory(asn1)
add_subdirectory(cu_cp)
add_subdirectory(cu_up)
add_subdirectory(du)
add_subdirectory(e1ap)
add_subdirectory(e2)
add_subdirectory(f1ap)
add_subdirectory(f1u)
add_subdirectory(fapi)
add_subdirectory(fapi_adaptor)
add_subdirectory(gateways)
add_subdirectory(gtpu)
if (DPDK_FOUND)
    add_subdirectory(hal)
endif (DPDK_FOUND)
add_subdirectory(instrumentation)
add_subdirectory(mac)
add_subdirectory(ngap)
add_subdirectory(nru)
if (DU_SPLIT_7_2 OR DU_SPLIT_DYNAMIC)
    add_subdirectory(ofh)
endif ()
add_subdirectory(pcap)
add_subdirectory(pdcp)
if (NOT DU_SPLIT_6)
    add_subdirectory(phy)
endif ()
add_subdirectory(psup)
if (DU_SPLIT_DYNAMIC OR DU_SPLIT_8)
    add_subdirectory(radio)
endif ()
add_subdirectory(ran)
add_subdirectory(rlc)
add_subdirectory(rrc)
if (NOT DU_SPLIT_6)
    add_subdirectory(ru)
endif ()
add_subdirectory(scheduler)
add_subdirectory(sdap)
add_subdirectory(security)
add_subdirectory(srslog)
add_subdirectory(srsvec)
add_subdirectory(support)
