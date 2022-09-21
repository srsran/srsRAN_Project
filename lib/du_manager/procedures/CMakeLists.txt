#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES initial_du_setup_procedure.cpp ue_creation_procedure.cpp ue_configuration_procedure.cpp)
add_library(srsgnb_du_manager_procedures STATIC ${SOURCES})
target_link_libraries(srsgnb_du_manager_procedures du_manager_ue srsgnb_ran)

