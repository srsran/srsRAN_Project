#!/bin/bash
#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

#
# This script attempts to automate some typical troubleshooting
# when using machines with lower processing power.
#
# It does the following:
# 1. Set the scaling governor to performance
# 2. Disable DRM KMS polling
# 3. Tune network buffers (Ethernet based USRPs only).
#

set -o errexit
set -o nounset
set -o pipefail

#
# Set the scaling governor to performance.
# See: https://docs.kernel.org/admin-guide/pm/cpufreq.html#generic-scaling-governors
#
set_performance_governor() {
  read -e -p "Would you like to set the CPU governor to performance: [Y/n]: " choice
  if ! [[ "$choice" == [Yy]* || "$choice" == "" ]]; then
    echo "Did not set scaling governor"
    return
  fi
  echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor >/dev/null
  echo "Scaling governor set to performance"
}

#
# DRM KMS polling can sometimes be expensive. Disabling it can sometimes help performance.
# See: https://wiki.archlinux.org/title/kernel_mode_setting#Problem_upon_bootloading_and_dmesg
#
set_kms_polling() {
  read -e -p "Would you like to disable KMS polling: [Y/n]: " choice
  if ! [[ "$choice" == [Yy]* || "$choice" == "" ]]; then
    echo "Did not disable KMS polling"
    return
  fi
  echo N | sudo tee /sys/module/drm_kms_helper/parameters/poll >/dev/null
  echo "Disabled DRM KMS polling"
}

#
# Set network buffer size, for USRPs connected over Ethernet.
# See: https://kb.ettus.com/USRP_Host_Performance_Tuning_Tips_and_Tricks
#
set_network_buffers() {
  read -e -p "Would you like to tweak the network buffer sizes: [Y/n]: " choice
  if ! [[ "$choice" == [Yy]* || "$choice" == "" ]]; then
    echo "Did not tweak network buffer sizes"
    return
  fi
  sudo sysctl -w net.core.wmem_max=33554432
  sudo sysctl -w net.core.rmem_max=33554432
  sudo sysctl -w net.core.wmem_default=33554432
  sudo sysctl -w net.core.rmem_default=33554432
  echo "Tweaked network buffer sizes"
}

#Check for sudo rights
sudo -v || exit

set_performance_governor
set_kms_polling
set_network_buffers
