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
from typing import Iterable

from retina.protocol.base_pb2 import Empty, UInteger
from retina.protocol.epc_pb2 import EPCStartInfo
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import UEAttachedInfo, UEStartInfo
from retina.protocol.ue_pb2_grpc import UEStub

ATTACH_TIMEOUT: int = 120
STARTUP_TIMEOUT: int = 3 * 60  # RF requires more time


def start_and_attach(
    ue: UEStub,  # pylint: disable=invalid-name
    gnb: GNBStub,
    epc: EPCStub,
    startup_timeout: int = STARTUP_TIMEOUT,
    attach_timeout: int = ATTACH_TIMEOUT,
) -> Iterable[UEAttachedInfo]:
    """
    Start stubs & wait until attach
    """

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

    return ue_attached_info_list.value  # type: ignore
