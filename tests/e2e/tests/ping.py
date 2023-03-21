#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Test ping
"""
import logging
from typing import Iterable, Union

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.base_pb2 import PingRequest
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import UEAttachedInfo
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import start_and_attach


@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(3, 15, 5, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s"),
        param(3, 15, 10, marks=(mark.zmq, mark.test), id="band:%s-scs:%s-bandwidth:%s"),
        param(3, 15, 20, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s"),
        param(3, 15, 50, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 10, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 20, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 50, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
# pylint: disable=too-many-arguments
def test_zmq(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    gnb: GNBStub,
    epc: EPCStub,
    band: int,
    common_scs: int,
    bandwidth: int,
):
    """
    ZMQ Pings
    """

    _ping(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue=ue,
        gnb=gnb,
        epc=epc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        global_timing_advance=0,
        time_alignment_calibration=0,
        log_search=True,
    )


@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(3, 15, 10, marks=mark.rf, id="band:%s-scs:%s-bandwidth:%s"),
        param(41, 30, 10, marks=mark.rf, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
# pylint: disable=too-many-arguments
def test_rf(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    gnb: GNBStub,
    epc: EPCStub,
    band: int,
    common_scs: int,
    bandwidth: int,
):
    """
    RF Pings
    """

    _ping(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue=ue,
        gnb=gnb,
        epc=epc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        log_search=False,
    )


# pylint: disable=too-many-arguments, too-many-locals
def _ping(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,  # pylint: disable=invalid-name
    gnb: GNBStub,
    epc: EPCStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    log_search: bool,
    always_download_artifacts: bool = False,
    ue_count: int = 4,
    ping_count: int = 10,
):
    logging.info("Ping Test")

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        ue_count=ue_count,
    )
    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
        log_search=log_search,
    )

    ue_attached_info_list: Iterable[UEAttachedInfo] = start_and_attach(ue, gnb, epc)

    # For each attached UE
    for ue_attached_info in ue_attached_info_list:
        # Launch both ping in parallel: ue -> epc and epc -> ue
        ue_to_epc = ue.Ping.future(PingRequest(address=ue_attached_info.ipv4_gateway, count=ping_count))
        epc_to_ue = epc.Ping.future(PingRequest(address=ue_attached_info.ipv4, count=ping_count))

        # Wait both ping to end
        ue_to_epc_result = ue_to_epc.result()
        epc_to_ue_result = epc_to_ue.result()

        # Print status
        logging.info("Ping [%s] UE -> EPC: %s", ue_attached_info.ipv4, ue_to_epc_result)
        logging.info("Ping [%s] EPC -> UE: %s", ue_attached_info.ipv4, epc_to_ue_result)

        # Validate both ping results
        assert all(map(lambda r: r.status, (ue_to_epc_result, epc_to_ue_result))) is True, "Ping failed!"
