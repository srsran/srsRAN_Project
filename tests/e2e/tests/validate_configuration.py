#
# Copyright 2021-2025 Software Radio Systems Limited
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
Validate Configuration Examples
"""
import logging
import tempfile
from pathlib import Path
from pprint import pformat
from time import sleep

from google.protobuf.empty_pb2 import Empty
from pytest import mark, param
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import FiveGCDefinition, PLMN, StartInfo, UEDefinition
from retina.protocol.fivegc_pb2 import FiveGCStartInfo
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub

from .steps.stub import RF_MAX_TIMEOUT, stop

N300_CONFIG_FILE: str = "configs/gnb_rf_n310_fdd_n3_20mhz.yml"


@mark.parametrize(
    "config_file",
    (
        param("configs/gnb_rf_b200_tdd_n78_20mhz.yml", id="tdd"),
        param("configs/gnb_rf_b210_fdd_srsUE.yml", id="fdd"),
    ),
)
@mark.rf_b200
@mark.flaky(
    reruns=2,
    only_rerun=[
        "socket is already closed",
        "failed to connect to all addresses",
    ],
)
def test_rf_b200_config(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    config_file: str,
    timeout: int = RF_MAX_TIMEOUT,
):  # pylint: disable=too-many-arguments,too-many-positional-arguments
    """
    Run gnb with B200 example config and validate it doesn't crash.
    """
    run_config(
        retina_manager=retina_manager,
        retina_data=retina_data,
        fivegc=fivegc,
        gnb=gnb,
        timeout=timeout,
        config_file=config_file,
        extra_config="",
    )


@mark.rf_n300
def test_rf_n300_config(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    timeout: int = RF_MAX_TIMEOUT,
):
    """
    Run gnb with N300 example config and validate it doesn't crash.
    """
    extra_config = "ru_sdr --clock external --sync external"
    run_config(
        retina_manager=retina_manager,
        retina_data=retina_data,
        fivegc=fivegc,
        gnb=gnb,
        timeout=timeout,
        config_file=N300_CONFIG_FILE,
        extra_config=extra_config,
    )


# pylint: disable=too-many-arguments,too-many-positional-arguments
def run_config(
    *,  # This enforces keyword-only arguments
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    timeout: int,
    config_file: str,
    extra_config: str,
):
    """
    Run gnb with B200 example config and validate it doesn't crash.
    """

    with tempfile.NamedTemporaryFile(mode="w+") as tmp_file:
        tmp_file.write(" ")  # Make it not empty to overwrite default one
        tmp_file.flush()
        retina_data.test_config = {
            "gnb": {
                "templates": {
                    "main": str(Path(__file__).joinpath(f"../../../../{config_file}").resolve()),
                    "cu": tmp_file.name,
                    "du": tmp_file.name,
                    "ru": tmp_file.name,
                }
            }
        }
        retina_manager.parse_configuration(retina_data.test_config)
        retina_manager.push_all_config()

    logging.info("Test config: \n%s", pformat(retina_data.test_config))

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=True,
    )

    plmn = PLMN(mcc="001", mnc="01")

    gnb.GetDefinition(Empty())
    fivegc_def: FiveGCDefinition = fivegc.GetDefinition(Empty())

    fivegc.Start(
        FiveGCStartInfo(
            plmn=plmn,
            start_info=StartInfo(timeout=timeout),
        )
    )
    logging.info("5GC started")

    gnb.Start(
        GNBStartInfo(
            plmn=plmn,
            ue_definition=UEDefinition(),
            fivegc_definition=fivegc_def,
            start_info=StartInfo(
                timeout=timeout,
                post_commands=(f"log --filename stdout {extra_config}",),
            ),
        )
    )
    logging.info("GNB started")

    # Sleep here because the N300 requires more time (~1minute) for initialization.
    # Otherwise we send SIGTERM to the gNB, it won't finish within 5s and will raise SIGKILL to itself,
    # causing this test to fail.
    logging.info("Waiting 60s...")
    sleep(60)

    stop(ue_array=tuple(), gnb=gnb, fivegc=fivegc, retina_data=retina_data, log_search=False)
