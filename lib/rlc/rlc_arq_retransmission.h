#ifndef RLC_ARQ_RETRANSMISSION
#define RLC_ARQ_RETRANSMISSION

#include "rlc_am_interconnect.h"
#include "rlc_am_pdu.h"
#include "rlc_rx_entity.h"
#include "rlc_rx_am_entity.h"
#include "sdu_window.h"

void rlc_arq_retransmit(
                        uint16_t tx_next_ack,
                        const sdu_window<rlc_tx_am_sdu_info>* tx_window,
                        srsran::rlc_am_status_pdu& status_pdu);
uint16_t get_highest_tx_sn_from_window(const sdu_window<rlc_tx_am_sdu_info>* tx_window);
void mark_for_retransmission(uint16_t nack_sn, const sdu_window<rlc_tx_am_sdu_info>* tx_window);

#endif