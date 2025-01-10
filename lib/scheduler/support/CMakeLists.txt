#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(sched_support STATIC
        pdcch/pdcch_type0_helpers.cpp
        pdsch/pdsch_default_time_allocation.cpp
        pdsch/pdsch_dmrs_symbol_mask.cpp
        pucch/pucch_default_resource.cpp
        pusch/pusch_default_time_allocation.cpp
        pusch/pusch_dmrs_symbol_mask.cpp
        mcs_tbs_calculator.cpp
        prbs_calculator.cpp
        mcs_calculator.cpp
        pdcch_aggregation_level_calculator.cpp
        rb_helper.cpp
        sr_helper.cpp
        dci_builder.cpp
        sch_pdu_builder.cpp
        csi_report_helpers.cpp
        pusch_power_controller.cpp
        pucch_power_controller.cpp)
