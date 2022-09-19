#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        gnb_cu_configuration_update_procedure.cpp
        f1ap_du_setup_procedure.cpp
        )

add_library(srsgnb_f1c_du_procedures STATIC ${SOURCES})
target_link_libraries(srsgnb_f1c_du_procedures f1ap_asn1)
