/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

// \brief Common signal handling methods for applications.

#pragma once

using srsran_signal_handler = void (*)();

/// Registers the specified function to be called when the user interrupts the program execution (eg: via Ctrl+C).
/// Passing a null function pointer disables the current installed handler.
void register_signal_handler(srsran_signal_handler handler);