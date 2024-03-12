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

"""
Validate Test Mode
"""

import logging
import tempfile
from pathlib import Path
from time import sleep

from pytest import mark, param
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import Empty, FiveGCDefinition, GNBDefinition, PLMN, StartInfo, UEDefinition
from retina.protocol.fivegc_pb2 import FiveGCStartInfo
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub

from .steps.stub import FIVEGC_STARTUP_TIMEOUT, GNB_STARTUP_TIMEOUT, handle_start_error, stop


@mark.parametrize(
    "extra_config, nof_ant",
    (
        param("test_mode test_ue --ri 1", 1, id="Test UE 1x1 Rank 1"),
        param("test_mode test_ue --ri 1", 2, id="Test UE 2x2 Rank 1"),
        param("test_mode test_ue --ri 2", 2, id="Test UE 2x2 Rank 2"),
        param("test_mode test_ue --ri 1", 4, id="Test UE 4x4 Rank 1"),
        param("test_mode test_ue --ri 2", 4, id="Test UE 4x4 Rank 2"),
        param("test_mode test_ue --ri 3", 4, id="Test UE 4x4 Rank 3"),
        param("test_mode test_ue --ri 4", 4, id="Test UE 4x4 Rank 4"),
    ),
)
@mark.test_mode
# pylint: disable=too-many-arguments
def test_ue(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    fivegc: FiveGCStub,
    gnb: GNBStub,
    # Test
    extra_config: str,
    nof_ant: int,
    duration: int = 5 * 60,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    fivegc_startup_timeout: int = FIVEGC_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
    fail_if_kos: bool = True,
):  # pylint: disable=too-many-locals
    """
    Run gnb in test mode.
    """

    # Configuration
    retina_data.test_config = {
        "gnb": {
            "parameters": {
                "gnb_id": 1,
                "log_level": "warning",
                "pcap": False,
            },
            "templates": {"cell": str(Path(__file__).joinpath("../test_mode/config_ue.yml").resolve())},
        },
    }
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    # 5GC and GNB Start
    with handle_start_error(name=f"5GC [{id(fivegc)}]"):
        fivegc_def: FiveGCDefinition = fivegc.GetDefinition(Empty())
        fivegc.Start(
            FiveGCStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                start_info=StartInfo(timeout=fivegc_startup_timeout),
            )
        )

    with handle_start_error(name=f"GNB [{id(gnb)}]"):
        gnb_def: GNBDefinition = gnb.GetDefinition(Empty())
        gnb.Start(
            GNBStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                ue_definition=UEDefinition(zmq_ip=gnb_def.zmq_ip, zmq_port_array=gnb_def.zmq_port_array[:nof_ant]),
                fivegc_definition=fivegc_def,
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    post_commands=f"cell_cfg --nof_antennas_dl {nof_ant} --nof_antennas_ul {nof_ant}"
                    + " "
                    + extra_config,
                ),
            )
        )

    logging.info("Running Test Mode for %s seconds", duration)
    sleep(duration)

    # Stop
    stop(
        tuple(),
        gnb,
        fivegc,
        retina_data,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        warning_as_errors=warning_as_errors,
        fail_if_kos=fail_if_kos,
    )


@mark.test_mode
# pylint: disable=too-many-arguments
def test_ru(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
    # Test
    nof_ant: int = 4,
    duration: int = 5 * 60,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
    fail_if_kos: bool = True,
):  # pylint: disable=too-many-locals
    """
    Run gnb in test mode.
    """

    # Configuration
    with tempfile.NamedTemporaryFile(mode="w+") as tmp_file:
        tmp_file.write(" ")  # Make it not empty to overwrite default one
        tmp_file.flush()
        retina_data.test_config = {
            "gnb": {
                "parameters": {
                    "gnb_id": 1,
                    "log_level": "warning",
                    "pcap": False,
                },
                "templates": {
                    "cell": str(Path(__file__).joinpath("../test_mode/config_ru.yml").resolve()),
                    "ru": tmp_file.name,
                },
            },
        }
        retina_manager.parse_configuration(retina_data.test_config)
        retina_manager.push_all_config()

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    # GNB Start
    with handle_start_error(name=f"GNB [{id(gnb)}]"):
        gnb_def: GNBDefinition = gnb.GetDefinition(Empty())
        gnb.Start(
            GNBStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                ue_definition=UEDefinition(zmq_ip=gnb_def.zmq_ip, zmq_port_array=gnb_def.zmq_port_array[:nof_ant]),
                fivegc_definition=FiveGCDefinition(amf_ip=gnb_def.zmq_ip, amf_port=38412),
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    post_commands="amf --no_core 1",
                ),
            )
        )

    logging.info("Running Test Mode for %s seconds", duration)
    sleep(duration)

    # Stop
    stop(
        tuple(),
        gnb,
        None,
        retina_data,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        warning_as_errors=warning_as_errors,
        fail_if_kos=fail_if_kos,
    )
