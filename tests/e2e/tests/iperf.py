#
# Copyright 2013-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Test ping
"""
import logging
from contextlib import suppress

import grpc
from pytest import mark
from retina.launcher.test_base import BaseTest
from retina.launcher.utils import param
from retina.protocol.base_pb2 import Empty, String, UInteger
from retina.protocol.epc_pb2 import EPCStartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto, IPerfRequest, UEStartInfo

from .utils import ATTACH_TIMEOUT, DEFAULT_MCS, STARTUP_TIMEOUT, get_ue_gnb_epc

SHORT_DURATION = 20
LONG_DURATION = 5 * 60
LOW_BITRATE = int(1e6)
HIGH_BITRATE = int(20e6)


class TestIPerf(BaseTest):
    @mark.parametrize(
        "ue_count",
        (
            param(1, id="singleue"),
            param(4, id="multiue", marks=mark.multiue),
        ),
    )
    @mark.parametrize(
        "direction",
        (
            param(IPerfDir.DOWNLINK, id="downlink", marks=mark.downlink),
            param(IPerfDir.UPLINK, id="uplink", marks=mark.uplink),
            param(IPerfDir.BIDIRECTIONAL, id="bidirectional", marks=mark.bidirectional),
        ),
    )
    @mark.parametrize(
        "protocol",
        (
            param(IPerfProto.UDP, id="udp", marks=mark.udp),
            param(IPerfProto.TCP, id="tcp", marks=mark.tcp),
        ),
    )
    @mark.parametrize(
        "band, common_scs, bandwidth, bitrate, iperf_duration",
        (
            # Smoke
            param(3, 15, 20, LOW_BITRATE, SHORT_DURATION, marks=mark.smoke, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(41, 30, 20, LOW_BITRATE, SHORT_DURATION, marks=mark.smoke, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            # ZMQ
            param(3, 15, 5, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(3, 15, 10, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(3, 15, 20, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(3, 15, 50, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(41, 30, 10, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(41, 30, 20, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(41, 30, 50, HIGH_BITRATE, SHORT_DURATION, marks=mark.zmq, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            # RF
            param(3, 15, 10, HIGH_BITRATE, LONG_DURATION, marks=mark.rf, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
            param(41, 30, 10, HIGH_BITRATE, LONG_DURATION, marks=mark.rf, id="band:%s-scs:%s-bandwidth:%s,bitrate:%s,duration:%s"),
        ),
    )
    def test(
        self,
        extra,
        band,
        common_scs,
        bandwidth,
        protocol,
        direction,
        iperf_duration,
        bitrate,
        ue_count,
        mcs=DEFAULT_MCS,
        startup_timeout=ATTACH_TIMEOUT,
        attach_timeout=STARTUP_TIMEOUT,
    ):
        logging.info("Iperf Test")

        with get_ue_gnb_epc(
            self, extra, band=band, common_scs=common_scs, bandwidth=bandwidth, mcs=mcs, ue_count=ue_count
        ) as items:
            ue, gnb, epc = items

            ue_def = ue.GetDefinition(Empty())
            gnb_def = gnb.GetDefinition(Empty())
            epc_def = epc.GetDefinition(Empty())

            for subscriber in ue_def.subscriber_list:
                epc.AddUESubscriber(subscriber)

            epc.Start(EPCStartInfo())
            logging.info("EPC started")

            gnb.Start(
                GNBStartInfo(
                    ue_definition=ue_def,
                    epc_definition=epc_def,
                    timeout=startup_timeout,
                )
            )
            logging.info("GNB started")

            ue.Start(
                UEStartInfo(
                    gnb_definition=gnb_def,
                    epc_definition=epc_def,
                    timeout=startup_timeout,
                )
            )
            logging.info("UE started")

            ue_attached_info_list = ue.WainUntilAttached(UInteger(value=attach_timeout))
            logging.info("UEs attached %s", ue_attached_info_list)

            for ue_attached_info in ue_attached_info_list.value:
                server = epc.StartIPerfService(String(value=ue_attached_info.ipv4_gateway))
                logging.info(
                    "\nIPerf %s [%s %s] in progress",
                    ue_attached_info.ipv4_gateway,
                    iperf_proto_to_str(protocol),
                    iperf_dir_to_str(direction),
                )

                with suppress(grpc.RpcError):
                    ue.IPerf(
                        IPerfRequest(
                            server=server,
                            duration=iperf_duration,
                            direction=direction,
                            proto=protocol,
                            bitrate=int(bitrate),
                        )
                    )

                iperf_data = epc.StopIPerfService(server)
                logging.info(
                    "Iperf %s [%s %s] result %s",
                    ue_attached_info.ipv4_gateway,
                    iperf_proto_to_str(protocol),
                    iperf_dir_to_str(direction),
                    iperf_data,
                )


def iperf_proto_to_str(proto):
    return {IPerfProto.TCP: "tcp", IPerfProto.UDP: "udp"}[proto]


def iperf_dir_to_str(direction):
    return {
        IPerfDir.DOWNLINK: "downlink",
        IPerfDir.UPLINK: "uplink",
        IPerfDir.BIDIRECTIONAL: "bidirectional",
    }[direction]
