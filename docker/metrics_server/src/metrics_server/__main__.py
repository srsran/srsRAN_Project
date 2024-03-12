#!/usr/bin/env python3

"""
Server to receive metrics data from a running srsRAN Project gnb and send it to the configured InfluxDB.
"""

import argparse
import json
import logging
import signal
import socket
from contextlib import suppress
from datetime import datetime
from http.client import RemoteDisconnected
from queue import Queue
from threading import Thread
from time import sleep
from typing import Any, Dict, Optional, Tuple

from influxdb_client import InfluxDBClient, WriteApi
from influxdb_client.client.write_api import SYNCHRONOUS


def main():
    """
    Main Entrypoint
    """

    client, bucket, testbed, clean_bucket, port, log_level = _parse_args()

    logging.basicConfig(format="%(asctime)s \x1b[32;20m[%(levelname)s]\x1b[0m %(message)s", level=log_level)
    logging.info("Starting srsRAN Project Metrics Server")

    if clean_bucket:
        _recreate_bucket(client, bucket)

    queue_obj: Queue[Optional[Dict[str, Any]]] = Queue()  #
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

    parsing_thread = Thread(target=_start_metric_server, args=(port, queue_obj))
    pushing_thread = Thread(target=_publish_data, args=(client, bucket, testbed, queue_obj))

    parsing_thread.start()
    pushing_thread.start()

    def close():
        logging.info("Closing")
        queue_obj.put(None)
        with suppress(IOError):
            socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM).sendto(b"", ("localhost", port))

    signal.signal(signal.SIGINT, lambda signum, frame: close())
    signal.signal(signal.SIGTERM, lambda signum, frame: close())

    parsing_thread.join()
    pushing_thread.join()

    logging.info("End")


def _parse_args() -> Tuple[InfluxDBClient, str, str, bool, int, int]:
    parser = argparse.ArgumentParser(
        description="Real time receiving and parsing srsRAN Project gnb metrics data and pushing it to influx db."
    )
    parser.add_argument("--port", type=int, required=True, help="Port to listen from.")
    parser.add_argument(
        "--db-config",
        nargs="*",
        required=True,
        help='Data base configuration in the format "key=value key=value"',
    )
    parser.add_argument("--bucket", required=True, help="Bucket to save the data.")
    parser.add_argument(
        "--clean-bucket", action="store_true", help="Remove all data in the bucket before pushing data from input file"
    )
    parser.add_argument("--testbed", required=True, help="Testbed where srsRAN Project was run")
    parser.add_argument(
        "--log-level", choices=logging.getLevelNamesMapping().keys(), default="INFO", help="Server Log level"
    )

    args = parser.parse_args()

    db_config = {key: value for pair_str in args.db_config for key, value in (pair_str.split("=", 1),)}

    return (
        InfluxDBClient(**db_config),
        args.bucket,
        args.testbed,
        args.clean_bucket,
        args.port,
        logging.getLevelNamesMapping()[args.log_level],
    )


def _start_metric_server(
    port: int,
    queue_obj: Queue[Optional[Dict[str, Any]]],  # pylint: disable=unsubscriptable-object
    max_buffer_size: int = 1024**2,
) -> None:
    # Create Server
    server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    except AttributeError:
        pass  # Some systems don't support SO_REUSEPORT
    server_socket.bind(("0.0.0.0", port))

    text = ""
    while True:
        line = server_socket.recv(max_buffer_size).decode()

        if not line:
            # If line was empty, end
            break
        text += line.strip()

        # Split jSONs
        header = ""
        *items, text = text.split("}{")
        for item in items:
            # Parse old items
            item = header + item + "}"
            try:
                queue_obj.put(json.loads(item))
            except json.JSONDecodeError:
                logging.error("Error decoding json: %s", item)
            header = "{"

        if text:
            text = header + text
            # Finally, try to parse remaining text
            with suppress(json.JSONDecodeError):
                queue_obj.put(json.loads(text))
                text = ""


def _publish_data(
    client: InfluxDBClient,
    bucket: str,
    testbed: str,
    queue_obj: Queue[Dict[str, Any]],  # pylint: disable=unsubscriptable-object
) -> None:
    with client.write_api(write_options=SYNCHRONOUS) as write_api:
        while True:
            metric = queue_obj.get(block=True)
            if metric is None:
                break
            try:
                # Currently we only support ue_list metric
                if "ue_list" in metric:
                    timestamp = datetime.utcfromtimestamp(metric["timestamp"]).isoformat()
                    # UE Info measurement
                    for ue_info in metric["ue_list"]:
                        ue_container = ue_info["ue_container"]
                        rnti = ue_container.pop("rnti")
                        pci = ue_container.pop("pci")
                        _influx_push(
                            write_api,
                            bucket=bucket,
                            record={
                                "measurement": "ue_info",
                                "tags": {
                                    "pci": pci,
                                    "rnti": f"{rnti:x}",
                                    "testbed": testbed,
                                },
                                "fields": dict(ue_container.items()),
                                "time": timestamp,
                            },
                            record_time_key="time",
                        )
                    logging.debug("Pushed %s", metric)
            except Exception as err:  # pylint: disable=broad-exception-caught
                logging.exception(err)


def _influx_push(write_api: WriteApi, *args, **kwargs) -> None:
    # It will block until the data is sent to the Remote
    while True:
        try:
            write_api.write(*args, **kwargs)
            break
        except (RemoteDisconnected, ConnectionRefusedError):
            logging.warning("Error pushing data. Retrying...")
            sleep(1)


def _recreate_bucket(client: InfluxDBClient, bucket_name: str) -> None:
    logging.info("Recreating the bucket")
    api = client.buckets_api()
    bucket_ref = api.find_bucket_by_name(bucket_name)
    api.delete_bucket(bucket_ref)
    api.create_bucket(bucket_ref)
    logging.info("Bucket cleaned")


if __name__ == "__main__":
    main()
