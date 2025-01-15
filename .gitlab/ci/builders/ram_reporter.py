#
# Copyright 2013-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Write RAM usage to a file until the process is killed
"""
import time
import os
import sys
import psutil

def get_cgroup_memory_path():
    try:
        with open('/proc/self/cgroup', 'r') as f:
            lines = f.readlines()
        for line in lines:
            parts = line.strip().split(':')
            if len(parts) == 3:
                cgroup_path = parts[2].strip()
                memory_path = f"/sys/fs/cgroup{cgroup_path}/memory.current"
                if os.path.exists(memory_path):
                    return memory_path
        return None
    except Exception as e:
        print(f"Error to read /proc/self/cgroup: {e}")
        return None

def get_memory_usage(memory_path):
    try:
        with open(memory_path, 'r') as f:
            memory_usage = int(f.read().strip())
        return memory_usage
    except FileNotFoundError:
        print(f"The file {memory_path} not found. Make sure that you use cgroups v2.")
        return None
    except Exception as e:
        print(f"Error to read {memory_path}: {e}")
        return None

def _bytes_to_gb(size_in_bytes):
    gb = size_in_bytes / (1024**3)
    return gb

def write_mem_baremetal():
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

def write_mem_kubernetes():
    memory_path = get_cgroup_memory_path()
    if not memory_path:
        print("Not cgroup memory path.")
        return

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
        current_ram_usage = get_memory_usage(memory_path)
        if current_ram_usage is not None:
            peak_ram_usage = max(peak_ram_usage, current_ram_usage)

            peak_ram_usage_gb = _bytes_to_gb(peak_ram_usage)

            with open(filename, "w", encoding="utf-8") as file:
                file.write(f"{peak_ram_usage_gb:.2f}")
        time.sleep(0.5)


if __name__ == "__main__":
    try:
        if len(sys.argv) == 3 and sys.argv[2] == "baremetal":
            write_mem_baremetal()
        else:
            write_mem_kubernetes()
    except Exception as e:
        print(f"Error: {e}")