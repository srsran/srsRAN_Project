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

add_subdirectory(asn1)
add_subdirectory(cu_cp)
add_subdirectory(cu_up)
add_subdirectory(du)
add_subdirectory(du_high)
add_subdirectory(du_manager)
add_subdirectory(e1ap)
add_subdirectory(e2)
add_subdirectory(f1ap)
add_subdirectory(f1u)
add_subdirectory(fapi)
add_subdirectory(fapi_adaptor)
add_subdirectory(gateways)
add_subdirectory(gtpu)
add_subdirectory(instrumentation)
if (DPDK_FOUND)
    add_subdirectory(hal)
endif (DPDK_FOUND)
add_subdirectory(mac)
add_subdirectory(ngap)
add_subdirectory(ofh)
add_subdirectory(pcap)
add_subdirectory(pdcp)
add_subdirectory(phy)
add_subdirectory(psup)
add_subdirectory(radio)
add_subdirectory(ran)
add_subdirectory(rlc)
add_subdirectory(rrc)
add_subdirectory(ru)
add_subdirectory(scheduler)
add_subdirectory(sdap)
add_subdirectory(security)
add_subdirectory(srslog)
add_subdirectory(srsvec)
add_subdirectory(support)
