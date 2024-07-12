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
KPI related logic
"""

from dataclasses import dataclass, fields
from typing import Optional, Sequence

from google.protobuf.empty_pb2 import Empty
from retina.launcher.public import MetricsSummary
from retina.protocol import RanStub
from retina.protocol.base_pb2 import Metrics
from retina.viavi.client import ViaviFailureManager


@dataclass
# pylint: disable=too-many-instance-attributes
class KPIs:
    """
    KPIs from a test
    """

    ul_brate_aggregate: float = 0
    dl_brate_aggregate: float = 0
    ul_brate_min: float = 0
    dl_brate_min: float = 0
    ul_brate_max: float = 0
    dl_brate_max: float = 0
    ul_bler_aggregate: float = 0
    dl_bler_aggregate: float = 0
    nof_ko_aggregate: int = 0
    nof_attach_failures: int = 0
    nof_reestablishments: int = 0
    nof_handovers: int = 0


# pylint: disable=too-many-locals
def get_kpis(
    gnb: RanStub,
    ue_array: Sequence[RanStub] = (),
    viavi_failure_manager: Optional[ViaviFailureManager] = None,
    metrics_summary: Optional[MetricsSummary] = None,
) -> KPIs:
    """
    Get KPIs from gnb, ue and viavi
    """

    kpis = KPIs()

    # GNB
    gnb_metrics: Metrics = gnb.GetMetrics(Empty())

    kpis.ul_brate_aggregate = gnb_metrics.total.ul_bitrate
    kpis.ul_brate_min = gnb_metrics.total.ul_bitrate_min
    kpis.ul_brate_max = gnb_metrics.total.ul_bitrate_max

    kpis.dl_brate_aggregate = gnb_metrics.total.dl_bitrate
    kpis.dl_brate_min = gnb_metrics.total.dl_bitrate_min
    kpis.dl_brate_max = gnb_metrics.total.dl_bitrate_max

    kpis.nof_ko_aggregate = gnb_metrics.total.dl_nof_ko + gnb_metrics.total.ul_nof_ko

    total_ul_ko_ok = gnb_metrics.total.ul_nof_ok + gnb_metrics.total.ul_nof_ko
    total_dl_ko_ok = gnb_metrics.total.dl_nof_ok + gnb_metrics.total.dl_nof_ko

    kpis.ul_bler_aggregate = 0 if not total_ul_ko_ok else gnb_metrics.total.ul_nof_ko / total_ul_ko_ok
    kpis.dl_bler_aggregate = 0 if not total_dl_ko_ok else gnb_metrics.total.dl_nof_ko / total_dl_ko_ok

    # UE
    for ue in ue_array:
        ue_metrics: Metrics = ue.GetMetrics(Empty())
        for ue_info in ue_metrics.ue_array:
            kpis.nof_reestablishments += ue_info.nof_reestablishments
            kpis.nof_handovers += ue_info.nof_handovers

    # Viavi
    if viavi_failure_manager:
        nof_failure = viavi_failure_manager.get_nof_failure_by_group_procedure("EMM_PROCEDURE", "attach")
        if nof_failure:
            kpis.nof_attach_failures += nof_failure

    # Save KPIs
    if metrics_summary is not None:
        for field in fields(kpis):
            field_value = getattr(kpis, field.name)
            metrics_summary.write_metric(field.name, field_value)

    return kpis
