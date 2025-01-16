/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_asn1_converters.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/ran/band_helper.h"

using namespace asn1::f1ap;

served_cell_info_s srsran::srs_du::make_asn1_served_cell_info(const du_served_cell_info& served_cell,
                                                              span<const s_nssai_t>      slices)
{
  served_cell_info_s f1ap_cell{};

  f1ap_cell.nr_cgi.plmn_id = served_cell.nr_cgi.plmn_id.to_bytes();
  f1ap_cell.nr_cgi.nr_cell_id.from_number(served_cell.nr_cgi.nci.value());
  f1ap_cell.nr_pci              = served_cell.pci;
  f1ap_cell.five_gs_tac_present = true;
  f1ap_cell.five_gs_tac.from_number(served_cell.tac);

  const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      MHz_to_bs_channel_bandwidth(served_cell.dl_carrier.carrier_bw_mhz), served_cell.scs_common, frequency_range::FR1);
  const double absolute_freq_point_a = band_helper::get_abs_freq_point_a_from_f_ref(
      band_helper::nr_arfcn_to_freq(served_cell.dl_carrier.arfcn_f_ref), nof_crbs, served_cell.scs_common);
  if (served_cell.duplx_mode == duplex_mode::TDD) {
    tdd_info_s& tdd           = f1ap_cell.nr_mode_info.set_tdd();
    tdd.nr_freq_info.nr_arfcn = band_helper::freq_to_nr_arfcn(absolute_freq_point_a);
    tdd.nr_freq_info.freq_band_list_nr.resize(1);
    tdd.nr_freq_info.freq_band_list_nr[0].freq_band_ind_nr = nr_band_to_uint(served_cell.dl_carrier.band);

    tdd.tx_bw.nr_scs.value = (nr_scs_opts::options)to_numerology_value(served_cell.scs_common);

    bool res = asn1::number_to_enum(tdd.tx_bw.nr_nrb, nof_crbs);
    srsran_assert(res, "Invalid number of CRBs for DL carrier BW");
  } else {
    fdd_info_s& fdd              = f1ap_cell.nr_mode_info.set_fdd();
    fdd.dl_nr_freq_info.nr_arfcn = band_helper::freq_to_nr_arfcn(absolute_freq_point_a);
    fdd.dl_nr_freq_info.freq_band_list_nr.resize(1);
    fdd.dl_nr_freq_info.freq_band_list_nr[0].freq_band_ind_nr = nr_band_to_uint(served_cell.dl_carrier.band);
    const double ul_absolute_freq_point_a                     = band_helper::get_abs_freq_point_a_from_f_ref(
        band_helper::nr_arfcn_to_freq(served_cell.ul_carrier->arfcn_f_ref), nof_crbs, served_cell.scs_common);
    fdd.ul_nr_freq_info.nr_arfcn = band_helper::freq_to_nr_arfcn(ul_absolute_freq_point_a);
    fdd.ul_nr_freq_info.freq_band_list_nr.resize(1);
    fdd.ul_nr_freq_info.freq_band_list_nr[0].freq_band_ind_nr = nr_band_to_uint(served_cell.ul_carrier->band);

    fdd.dl_tx_bw.nr_scs.value = (nr_scs_opts::options)to_numerology_value(served_cell.scs_common);
    unsigned nof_dl_crbs =
        band_helper::get_n_rbs_from_bw(MHz_to_bs_channel_bandwidth(served_cell.dl_carrier.carrier_bw_mhz),
                                       served_cell.scs_common,
                                       frequency_range::FR1);
    bool res = asn1::number_to_enum(fdd.dl_tx_bw.nr_nrb, nof_dl_crbs);
    srsran_assert(res, "Invalid number of CRBs for DL carrier BW");
    fdd.ul_tx_bw.nr_scs.value = (nr_scs_opts::options)to_numerology_value(served_cell.scs_common);
    unsigned nof_ul_crbs =
        band_helper::get_n_rbs_from_bw(MHz_to_bs_channel_bandwidth(served_cell.ul_carrier->carrier_bw_mhz),
                                       served_cell.scs_common,
                                       frequency_range::FR1);
    res = asn1::number_to_enum(fdd.ul_tx_bw.nr_nrb, nof_ul_crbs);
    srsran_assert(res, "Invalid number of CRBs for DL carrier BW");
  }
  f1ap_cell.meas_timing_cfg = served_cell.packed_meas_time_cfg.copy();

  // Fill Served PLMNs
  f1ap_cell.served_plmns.resize(1);
  auto plmn_bytes                   = served_cell.nr_cgi.plmn_id.to_bytes();
  f1ap_cell.served_plmns[0].plmn_id = plmn_bytes;

  // Fill slicing information.
  f1ap_cell.served_plmns[0].ie_exts_present                        = not slices.empty();
  f1ap_cell.served_plmns[0].ie_exts.tai_slice_support_list_present = not slices.empty();
  for (const s_nssai_t& s_nssai : slices) {
    slice_support_item_s slice{};
    slice.snssai.sst.from_number(s_nssai.sst.value());
    slice.snssai.sd_present = s_nssai.sd.is_set();
    if (slice.snssai.sd_present) {
      slice.snssai.sd.from_number(s_nssai.sd.value());
    }
    f1ap_cell.served_plmns[0].ie_exts.tai_slice_support_list.push_back(slice);
  }

  return f1ap_cell;
}
