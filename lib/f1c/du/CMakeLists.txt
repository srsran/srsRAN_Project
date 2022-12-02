#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(procedures)
add_library(srsgnb_f1c_du
        f1ap_du_impl.cpp
        f1ap_du_factory.cpp
        f1ap_du_ue.cpp
        f1c_du_bearer_impl.cpp)
target_include_directories(srsgnb_f1c_du PRIVATE ..)
target_link_libraries(srsgnb_f1c_du f1ap_asn1 srsgnb_f1c_common srsgnb_f1c_du_procedures)
