add_test( cu_du_test.when_f1setup_successful_then_du_connected /home/shovon/srsRAN_Project/build/tests/integrationtests/du_high_cu/cu_du_test [==[--gtest_filter=cu_du_test.when_f1setup_successful_then_du_connected]==] --gtest_also_run_disabled_tests)
set_tests_properties( cu_du_test.when_f1setup_successful_then_du_connected PROPERTIES WORKING_DIRECTORY /home/shovon/srsRAN_Project/build/tests/integrationtests/du_high_cu SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( cu_du_test_TESTS cu_du_test.when_f1setup_successful_then_du_connected)
