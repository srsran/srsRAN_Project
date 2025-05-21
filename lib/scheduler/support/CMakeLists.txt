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

add_library(sched_support STATIC
        pdcch/pdcch_type0_helpers.cpp
        pdsch/pdsch_default_time_allocation.cpp
        pdsch/pdsch_dmrs_symbol_mask.cpp
        pdsch/pdsch_config_params.cpp
        pucch/pucch_default_resource.cpp
        pusch/pusch_default_time_allocation.cpp
        pusch/pusch_dmrs_symbol_mask.cpp
	paging_helpers.cpp
        mcs_tbs_calculator.cpp
        prbs_calculator.cpp
        mcs_calculator.cpp
        pdcch_aggregation_level_calculator.cpp
        sr_helper.cpp
        dci_builder.cpp
        sch_pdu_builder.cpp
        csi_report_helpers.cpp
        pusch_power_controller.cpp
        pucch_power_controller.cpp)
