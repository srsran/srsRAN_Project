#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Write RAM usage to a file until the process is killed
"""

import sys
import time

import psutil


def _bytes_to_gb(size_in_bytes):
    gb = size_in_bytes / (1024**3)
    return gb


def _main():
    filename = sys.argv[1]
    peak_ram_usage = 0

    try:
        with open(filename, "r", encoding="utf-8") as file:
            previous_content = file.read()
            previous_peak_ram_usage = float(previous_content)
            peak_ram_usage = previous_peak_ram_usage
    except FileNotFoundError:
        peak_ram_usage = 0

    while True:
        current_ram_usage = psutil.virtual_memory().used
        peak_ram_usage = max(peak_ram_usage, current_ram_usage)

        peak_ram_usage_gb = _bytes_to_gb(peak_ram_usage)

        with open(filename, "w", encoding="utf-8") as file:
            file.write(f"{peak_ram_usage_gb:.2f}")

        time.sleep(0.5)


if __name__ == "__main__":
    _main()
