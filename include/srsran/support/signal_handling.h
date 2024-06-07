/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

using srsran_signal_handler = void (*)();

/// \brief Registers the specified function to be called when the user interrupts the program execution (eg: via
/// Ctrl+C).
///
/// Passing a null function pointer disables the current installed handler.
void register_interrupt_signal_handler(srsran_signal_handler handler);

/// \brief Registers the specified function to be called when the application is about to be forcefully shutdown by an
/// alarm signal.
///
/// Passing a null function pointer disables the current installed handler.
void register_cleanup_signal_handler(srsran_signal_handler handler);

} // namespace srsran
