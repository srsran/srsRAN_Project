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
from retina.protocol.base_pb2 import Empty, EPCDefinition, GNBDefinition, StartInfo, UEDefinition
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub

from .steps.stub import RF_MAX_TIMEOUT

B200_CONFIG_FILE: str = "configs/gnb_rf_b200_tdd_n78_20mhz.yml"


@mark.rf_b200
def test_rf_b200_config(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    epc: EPCStub,
    gnb: GNBStub,
    timeout: int = RF_MAX_TIMEOUT,
):
    """
    Run gnb with B200 example config and validate it doesn't crash.
    """

    retina_data.test_config = {
        "gnb": {"templates": {"main": str(Path(__file__).joinpath(f"../../../../{B200_CONFIG_FILE}").resolve())}}
    }
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()
    logging.info("Test config: \n%s", pformat(retina_data.test_config))

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=True,
        log_search=False,
    )

    gnb_def: GNBDefinition = gnb.GetDefinition(Empty())
    epc_def: EPCDefinition = epc.GetDefinition(Empty())

    epc.Start(StartInfo(timeout=timeout))
    logging.info("EPC started")

    gnb.Start(
        GNBStartInfo(
            ue_definition=UEDefinition(),
            epc_definition=epc_def,
            start_info=StartInfo(
                timeout=timeout,
                post_commands=f"amf --addr {epc_def.amf_ip} --bind_addr {gnb_def.zmq_ip} log --filename stdout",
            ),
        )
    )
    logging.info("GNB started")
