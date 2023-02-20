#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES initial_du_setup_procedure.cpp ue_creation_procedure.cpp ue_configuration_procedure.cpp)
add_library(srsran_du_manager_procedures STATIC ${SOURCES})
target_link_libraries(srsran_du_manager_procedures sched_config srsran_ran mac_configuration_helpers du_ue)

