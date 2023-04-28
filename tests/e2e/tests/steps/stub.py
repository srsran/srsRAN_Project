#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
"""
Steps related with stubs / resources
"""

import logging
from contextlib import suppress
from typing import Dict, Sequence, Tuple

import grpc
from retina.protocol.base_pb2 import Empty, PingRequest, StartInfo, String, UEDefinition, UInteger
from retina.protocol.epc_pb2 import IPerfResponse
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto, IPerfRequest, UEAttachedInfo, UEStartInfo
from retina.protocol.ue_pb2_grpc import UEStub

RF_MAX_TIMEOUT: int = 3 * 60  # Time enough in RF when loading a new image in the sdr
UE_STARTUP_TIMEOUT: int = RF_MAX_TIMEOUT
GNB_STARTUP_TIMEOUT: int = 5  # GNB delay (we wait x seconds and check it's still alive). UE later and has a big timeout
EPC_STARTUP_TIMEOUT: int = RF_MAX_TIMEOUT
ATTACH_TIMEOUT: int = 120


# pylint: disable=too-many-arguments,too-many-locals
def start_and_attach(
    ue_array: Sequence[UEStub],
    gnb: GNBStub,
    epc: EPCStub,
    ue_startup_timeout: int = UE_STARTUP_TIMEOUT,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    epc_startup_timeout: int = EPC_STARTUP_TIMEOUT,
    gnb_pre_cmd: str = "",
    gnb_post_cmd: str = "",
    attach_timeout: int = ATTACH_TIMEOUT,
) -> Dict[UEStub, UEAttachedInfo]:
    """
    Start stubs & wait until attach
    """

    ue_def_for_gnb = UEDefinition()
    for ue_stub in ue_array:
        ue_def: UEDefinition = ue_stub.GetDefinition(Empty())
        epc.AddUESubscriber(ue_def.subscriber)
        if ue_def.zmq_ip is not None:
            ue_def_for_gnb = ue_def

    # EPC Start
    epc.Start(StartInfo(timeout=epc_startup_timeout))
    logging.info("EPC [%s] started", id(epc))

    # GNB Start
    gnb.Start(
        GNBStartInfo(
            ue_definition=ue_def_for_gnb,
            epc_definition=epc.GetDefinition(Empty()),
            start_info=StartInfo(
                timeout=gnb_startup_timeout,
                pre_commands=gnb_pre_cmd,
                post_commands=gnb_post_cmd,
            ),
        )
    )
    logging.info("GNB [%s] started", id(gnb))

    return ue_start_and_attach(ue_array, gnb, epc, ue_startup_timeout=ue_startup_timeout, attach_timeout=attach_timeout)


def ue_start_and_attach(
    ue_array: Sequence[UEStub],
    gnb: GNBStub,
    epc: EPCStub,
    ue_startup_timeout: int = UE_STARTUP_TIMEOUT,
    attach_timeout: int = ATTACH_TIMEOUT,
) -> Dict[UEStub, UEAttachedInfo]:
    """
    Start an array of UEs and wait until attached to already running gnb and epc
    """

    for ue_stub in ue_array:
        ue_stub.Start(
            UEStartInfo(
                gnb_definition=gnb.GetDefinition(Empty()),
                epc_definition=epc.GetDefinition(Empty()),
                start_info=StartInfo(timeout=ue_startup_timeout),
            )
        )
        logging.info("UE [%s] started", id(ue_stub))

    # Attach in parallel
    ue_attach_task_dict: Dict[UEStub, grpc.Future] = {
        ue_stub: ue_stub.WainUntilAttached.future(UInteger(value=attach_timeout)) for ue_stub in ue_array
    }

    ue_attach_info_dict: Dict[UEStub, UEAttachedInfo] = {
        ue_stub: task.result() for ue_stub, task in ue_attach_task_dict.items()
    }

    for ue_stub, ue_attach_info in ue_attach_info_dict.items():
        logging.info(
            "UE [%s] attached: \n%s%s ",
            id(ue_stub),
            ue_stub.GetDefinition(Empty()).subscriber,
            ue_attach_info,
        )

    return ue_attach_info_dict


def ping(
    ue_attach_info_dict: Dict[UEStub, UEAttachedInfo],
    epc: EPCStub,
    ping_count,
):
    """
    Ping command between an UE and a EPC
    """

    # For each attached UE
    for ue_stub, ue_attached_info in ue_attach_info_dict.items():
        # Launch both ping in parallel: ue -> epc and epc -> ue
        ue_to_epc = ue_stub.Ping.future(PingRequest(address=ue_attached_info.ipv4_gateway, count=ping_count))
        epc_to_ue = epc.Ping.future(PingRequest(address=ue_attached_info.ipv4, count=ping_count))

        # Wait both ping to end
        ue_to_epc_result = ue_to_epc.result()
        epc_to_ue_result = epc_to_ue.result()

        # Print status
        logging.info("Ping [%s] UE -> EPC: %s", ue_attached_info.ipv4, ue_to_epc_result)
        logging.info("Ping [%s] EPC -> UE: %s", ue_attached_info.ipv4, epc_to_ue_result)

        # Validate both ping results
        assert all(map(lambda r: r.status, (ue_to_epc_result, epc_to_ue_result))) is True, "Ping failed!"


def iperf(
    ue_attach_info_dict: Dict[UEStub, UEAttachedInfo],
    epc: EPCStub,
    protocol: IPerfProto,
    direction: IPerfDir,
    iperf_duration: int,
    bitrate: int,
    bitrate_threshold_ratio: float,  # real_bitrate > (bitrate_threshold_ratio * ideal_bitrate)
):
    """
    iperf command between an UE and a EPC
    """

    # For each attached UE
    for ue_stub, ue_attached_info in ue_attach_info_dict.items():
        # Start IPerf Server
        task, iperf_request = iperf_start(ue_stub, ue_attached_info, epc, protocol, direction, iperf_duration, bitrate)
        iperf_wait_until_finish(ue_attached_info, epc, task, iperf_request, bitrate_threshold_ratio)


def iperf_start(
    ue: UEStub,  # pylint: disable=invalid-name
    ue_attached_info: UEAttachedInfo,
    epc: EPCStub,
    protocol: IPerfProto,
    direction: IPerfDir,
    duration: int,
    bitrate: int,
) -> Tuple[grpc.Future, IPerfRequest]:
    """
    Start a Iperf and keep it running
    """

    iperf_request = IPerfRequest(
        server=epc.StartIPerfService(String(value=ue_attached_info.ipv4_gateway)),
        duration=duration,
        direction=direction,
        proto=protocol,
        bitrate=bitrate,
    )

    # Run iperf
    task = ue.IPerf.future(iperf_request)

    logging.info(
        "Iperf %s [%s %s] started",
        ue_attached_info.ipv4,
        _iperf_proto_to_str(iperf_request.proto),
        _iperf_dir_to_str(iperf_request.direction),
    )

    return (task, iperf_request)


def iperf_wait_until_finish(
    ue_attached_info: UEAttachedInfo,
    epc: EPCStub,
    task: grpc.Future,
    iperf_request: IPerfRequest,
    bitrate_threshold_ratio: float,  # real_bitrate > (bitrate_threshold_ratio * ideal_bitrate)
):
    """
    Wait until the requested iperf has finished.
    """

    # Stop server, get results and print it
    with suppress(grpc.RpcError):
        task.result()
    iperf_data: IPerfResponse = epc.StopIPerfService(iperf_request.server)
    logging.info(
        "Iperf %s [%s %s] result %s",
        ue_attached_info.ipv4,
        _iperf_proto_to_str(iperf_request.proto),
        _iperf_dir_to_str(iperf_request.direction),
        iperf_data,
    )

    # Assertion
    if iperf_request.direction in (IPerfDir.DOWNLINK, IPerfDir.BIDIRECTIONAL):
        if iperf_data.downlink.bits_per_second < bitrate_threshold_ratio * iperf_request.bitrate:
            logging.warning(
                "Downlink bitrate too low. Requested: %s - Measured: %s",
                iperf_request.bitrate,
                iperf_data.downlink.bits_per_second,
            )
    if iperf_request.direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
        if iperf_data.uplink.bits_per_second < bitrate_threshold_ratio * iperf_request.bitrate:
            logging.warning(
                "Uplink bitrate too low. Requested: %s - Measured: %s",
                iperf_request.bitrate,
                iperf_data.uplink.bits_per_second,
            )


def _iperf_proto_to_str(proto):
    return {IPerfProto.TCP: "tcp", IPerfProto.UDP: "udp"}[proto]


def _iperf_dir_to_str(direction):
    return {
        IPerfDir.DOWNLINK: "downlink",
        IPerfDir.UPLINK: "uplink",
        IPerfDir.BIDIRECTIONAL: "bidirectional",
    }[direction]
