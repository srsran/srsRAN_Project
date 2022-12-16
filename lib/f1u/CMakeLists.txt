#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_f1u_common common/f1u_local_connector_impl.cpp common/f1u_connector_factory.cpp)
add_library(srsgnb_f1u_cu_up cu_up/f1u_bearer_impl.cpp cu_up/f1u_bearer_factory.cpp)
add_library(srsgnb_f1u_du du/f1u_bearer_impl.cpp du/f1u_bearer_factory.cpp)
