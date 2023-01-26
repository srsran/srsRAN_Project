#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
      cu_up_factory.cpp
      cu_up_impl.cpp
      ue_manager.cpp
      pdu_session_manager_impl.cpp
)

add_library(srsgnb_cu_up STATIC ${SOURCES})

target_link_libraries(srsgnb_cu_up srsgnb_e1 srsgnb_gtpu srsgnb_pdcp  srsgnb_gateway e1ap_asn1 srsran_sdap srsgnb_security  srsgnb_support srslog )
