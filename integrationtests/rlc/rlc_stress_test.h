/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/srslog/srslog.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <random>

struct stress_test_args {
  std::string mode = "UM";
};

void parse_args(stress_test_args& args, int argc, char* argv[]) {}

class mac_dummy
{
public:
  explicit mac_dummy(stress_test_args args_) {}
};

class rlc_tester
{
public:
  rlc_tester() {}

private:
};

