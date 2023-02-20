#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_f1u_cu_up cu_up/f1u_bearer_impl.cpp cu_up/f1u_bearer_factory.cpp)
add_library(srsran_f1u_du du/f1u_bearer_impl.cpp du/f1u_bearer_factory.cpp)
