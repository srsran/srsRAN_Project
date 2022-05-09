#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(dl_ssb_pdu_validator_test dl_ssb_pdu_test.cpp)
target_link_libraries(dl_ssb_pdu_validator_test srsgnb_support srslog fapi_validators)
add_test(dl_ssb_pdu_validator_test dl_ssb_pdu_validator_test)

add_executable(dl_tti_request_validator_test dl_tti_request_test.cpp)
target_link_libraries(dl_tti_request_validator_test srsgnb_support srslog fapi_validators)
add_test(dl_tti_request_validator_test dl_tti_request_validator_test)

add_executable(dl_pdcch_pdu_validator_test dl_pdcch_pdu_test.cpp)
target_link_libraries(dl_pdcch_pdu_validator_test srsgnb_support srslog fapi_validators)
add_test(dl_pdcch_pdu_validator_test dl_pdcch_pdu_validator_test)