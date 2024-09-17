#include "rlc_arq_retransmission.h"

void rlc_arq_retransmit(
                        uint16_t tx_next_ack,
                        const sdu_window<rlc_tx_am_sdu_info>* tx_window,
                        srsran::rlc_am_status_pdu& status_pdu)
{
    const auto& nacks = status_pdu.get_nacks();
    uint16_t highest_tx_sn = get_highest_tx_sn_from_window(tx_window);

    if (nacks.empty()) {
        return;
    }

    for (size_t nack_idx = 0; nack_idx < nacks.size(); ++nack_idx) {
        const auto& nack = nacks[nack_idx];
        const auto* prev_nack = (nack_idx > 0) ? &nacks[nack_idx - 1] : nullptr;
        const auto* next_nack = (nack_idx < nacks.size() - 1) ? &nacks[nack_idx + 1] : nullptr;

        uint16_t nack_sn = nack.nack_sn;

        if (tx_next_ack <= nack_sn && nack_sn <= highest_tx_sn) {
            mark_for_retransmission(nack_sn, tx_window);
        }
    }
}

uint16_t get_highest_tx_sn_from_window(const sdu_window<rlc_tx_am_sdu_info>* tx_window) {
    uint16_t highest_sn = INVALID_RLC_SN;
    if (tx_window) {
        for (const auto& entry : *tx_window) {
            if (entry.is_valid() && entry.sn > highest_sn) {
                highest_sn = entry.sn;
            }
        }
    }
    return highest_sn;
}

void mark_for_retransmission(uint16_t nack_sn, const sdu_window<rlc_tx_am_sdu_info>* tx_window) {
    if (!tx_window) {
        return;
    }
    for (auto& sdu_info : *tx_window) {
        if (sdu_info.sn == nack_sn && sdu_info.is_valid()) {
            sdu_info.is_retx = true;
            break;
        }
    }
}

