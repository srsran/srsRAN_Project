#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Validate Configuration Examples
"""
import logging
from pathlib import Path
from pprint import pformat

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import Empty, FiveGCDefinition, GNBDefinition, PLMN, StartInfo, UEDefinition
from retina.protocol.fivegc_pb2 import FiveGCStartInfo
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub

from .steps.stub import RF_MAX_TIMEOUT, stop

B200_CONFIG_FILE: str = "configs/gnb_rf_b200_tdd_n78_20mhz.yml"
N300_CONFIG_FILE: str = "configs/gnb_rf_n310_fdd_n3_20mhz.yml"


@mark.rf_b200
def test_rf_b200_config(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    timeout: int = RF_MAX_TIMEOUT,
):
    """
    Run gnb with B200 example config and validate it doesn't crash.
    """
    run_config(retina_manager, retina_data, fivegc, gnb, timeout, B200_CONFIG_FILE, "")


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
    run_config(retina_manager, retina_data, fivegc, gnb, timeout, N300_CONFIG_FILE, extra_config)


# pylint: disable=R0913
def run_config(
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

    retina_data.test_config = {
        "gnb": {"templates": {"main": str(Path(__file__).joinpath(f"../../../../{config_file}").resolve())}}
    }
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()
    logging.info("Test config: \n%s", pformat(retina_data.test_config))

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=True,
    )

    plmn = PLMN(mcc="001", mnc="01")

    gnb_def: GNBDefinition = gnb.GetDefinition(Empty())
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
                post_commands=(
                    f"amf --addr {fivegc_def.amf_ip} --bind_addr {gnb_def.zmq_ip} "
                    f"log --filename stdout {extra_config}"
                ),
            ),
        )
    )
    logging.info("GNB started")

    stop(tuple(), gnb, fivegc, retina_data, log_search=False)
