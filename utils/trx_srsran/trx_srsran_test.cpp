/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <dlfcn.h>
#include <iostream>

#include <set>

extern "C" {
#include <trx_driver.h>
}

#define TESTASSERT(COND)                                                                                               \
  do {                                                                                                                 \
    if (!(COND)) {                                                                                                     \
      std::cerr << "Error in " << __FILE__ << ":" << __LINE__ << ":"                                                   \
                << ": unmet condition '" << #COND << "'" << std::endl;                                                 \
      std::abort();                                                                                                    \
    }                                                                                                                  \
  } while (false)

static unsigned              trx_app_opaque = 0x1234;
static std::set<std::string> trx_param_names;
static char*                 trx_get_param_string(void* app_opaque, const char* prop_name)
{
  TESTASSERT(&trx_app_opaque == app_opaque);
  trx_param_names.insert(prop_name);
  return nullptr;
}

static int trx_get_param_double(void* app_opaque, double* param, const char* prop_name)
{
  TESTASSERT(&trx_app_opaque == app_opaque);
  trx_param_names.insert(prop_name);
  return -1;
}

int main(int argc, char** argv)
{
  void* handle;
  char* error;
  int (*driver_init)(TRXState * s);

  // Check number of inputs
  if (argc != 2) {
    std::cerr << "Expected usage: " << argv[0] << " library.so" << std::endl;
    return -1;
  }

  handle = dlopen(argv[1], RTLD_NOW);
  if (handle == nullptr) {
    std::cerr << "Error: " << dlerror() << std::endl;
    exit(EXIT_FAILURE);
  }

  // Clear any existing error.
  dlerror();

  // Load init function.
  driver_init = (int (*)(TRXState*))dlsym(handle, "trx_driver_init");

  error = dlerror();
  if (error != nullptr) {
    std::cerr << "Error: " << error << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "Dynamic library loaded." << std::endl;

  TRXState state             = {};
  state.trx_api_version      = TRX_API_VERSION;
  state.app_opaque           = static_cast<void*>(&trx_app_opaque);
  state.trx_get_param_string = &trx_get_param_string;
  state.trx_get_param_double = &trx_get_param_double;
  driver_init(&state);
  std::cout << "Driver initialised successfully." << std::endl;

  // Make sure parameters are parsed.
  TESTASSERT(trx_param_names.count("factory"));
  TESTASSERT(trx_param_names.count("log_level"));
  TESTASSERT(trx_param_names.count("otw_format"));
  TESTASSERT(trx_param_names.count("rx_port0"));
  TESTASSERT(trx_param_names.count("tx_port0"));
  TESTASSERT(trx_param_names.count("noise_spd"));
  std::cout << "Driver parsed parameters." << std::endl;

  // Make sure the init function set the methods.
  TESTASSERT(state.trx_get_sample_rate_func);
  TESTASSERT(state.trx_start_func);
  TESTASSERT(state.trx_write_func == nullptr);
  TESTASSERT(state.trx_read_func == nullptr);
  TESTASSERT(state.trx_set_tx_gain_func);
  TESTASSERT(state.trx_set_rx_gain_func);
  TESTASSERT(state.trx_end_func);
  TESTASSERT(state.trx_get_tx_samples_per_packet_func);
  TESTASSERT(state.trx_get_stats);
  TESTASSERT(state.trx_dump_info);
  TESTASSERT(state.trx_get_abs_tx_power_func == nullptr);
  TESTASSERT(state.trx_get_abs_rx_power_func == nullptr);
  TESTASSERT(state.trx_write_func2);
  TESTASSERT(state.trx_read_func2);
  TESTASSERT(state.trx_msg_recv_func == nullptr);
  TESTASSERT(state.trx_msg_send_func == nullptr);
  TESTASSERT(state.trx_get_tx_gain_func);
  TESTASSERT(state.trx_get_rx_gain_func);
  TESTASSERT(state.trx_stop_func == nullptr);
  std::cout << "Driver set callbacks." << std::endl;

  dlclose(handle);

  std::cout << "Successful." << std::endl;
  exit(EXIT_SUCCESS);
}