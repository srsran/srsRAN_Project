add_test( HardDecision.valid_results /home/shovon/srsRAN_Project/build/tests/unittests/phy/upper/hard_decision_test [==[--gtest_filter=HardDecision.valid_results]==] --gtest_also_run_disabled_tests)
set_tests_properties( HardDecision.valid_results PROPERTIES WORKING_DIRECTORY /home/shovon/srsRAN_Project/build/tests/unittests/phy/upper SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( hard_decision_test_TESTS HardDecision.valid_results)
