#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
      cu_cp_factory.cpp
      cu_cp.cpp
      du_processor_impl.cpp
      du_processor_factory.cpp
      ue_manager.cpp
      procedures/initial_cu_cp_setup_procedure.cpp
)

add_library(srsgnb_cu_cp STATIC ${SOURCES})

target_link_libraries(srsgnb_cu_cp srslog srsgnb_support srsgnb_f1c srsgnb_pdcp srsgnb_ngap srsgnb_rrc srsgnb_security rrc_nr_asn1)
