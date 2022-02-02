/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CHANNEL_CODING_POLAR_DECODER_IMPL_H
#define SRSGNB_CHANNEL_CODING_POLAR_DECODER_IMPL_H

#include "srsgnb/phy/channel_coding/polar_decoder.h"
#include "srsgnb/phy/channel_coding/polar_encoder.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

class polar_decoder_impl : public polar_decoder
{
private:
  /*!
   * \brief Types of node in an SSC decoder.
   */
  typedef enum {
    RATE_0 = 0, /*!< \brief See function rate_0_node(). */
    RATE_R = 2, /*!< \brief See function rate_r_node(). */
    RATE_1 = 3, /*!< \brief See function rate_1_node(). */
  } node_rate;

  /*!
   * \brief Stores constants.
   */
  struct params_s {
    uint8_t code_size_log; /*!< \brief \f$log_2\f$ of code size. */
    srsvec::aligned_vec<uint16_t>
             code_stage_size; /*!< \brief Number of bits of the encoder input/output vector at a given stage. */
    uint16_t frozen_set_size; /*!< \brief Number of frozen bits. */
    std::vector<uint8_t>
        node_type_alloc; /*!< \brief Node type indicator 1 at all stages 3 (rate-1), 2 (rate-r), 0 (rate-0). */
    std::vector<uint8_t*>
        node_type; /*!< \brief Node type indicator 1 at all stages 3 (rate-1), 2 (rate-r), 0 (rate-0). */
  };

  /*!
   * \brief Describes the state of a SSC polar decoder
   */
  struct state_s {
    uint8_t                       stage; /*!< \brief Current stage [0 - code_size_log] of the decoding algorithm. */
    bool                          flag_finished; /*!< \brief True if the last bit is decoded. False otherwise. */
    srsvec::aligned_vec<uint16_t> active_node_per_stage; /*!< \brief Indicates the active node in each stage of the
                                        algorithm at a given moment. */
  };

  /*!
   * \brief Structure with pointers needed to obtain the node_type
   */
  struct tmp_node_s {
    srsvec::aligned_vec<uint8_t>  is_not_rate_0; /*!< \brief Pointers to a temporary buffer. */
    span<uint8_t>                 is_rate_1;     /*!< \brief Pointers to a temporary buffer. */
    srsvec::aligned_vec<uint16_t> i_even;        /*!< \brief Pointers to a temporary buffer. */
    srsvec::aligned_vec<uint16_t> i_odd;         /*!< \brief Pointers to a temporary buffer. */

    /*!
     * Allocates memory resources for the computation of the node_type.
     * \param[in] nMax \f$log_2\f$ of the maximum number of bits in the codeword.
     */
    tmp_node_s(uint8_t nMax);

    /*!
     * Computes node types for the decoding tree associated to the given frozen set.
     * \param[in] p Pointer of a Tmp_node_type structure with the memory resources needed.
     * \param[out] node_type Double pointer containing the node type at each stage of the decoding tree.
     * \param[in] code_size_log \f$log_2\f$ of code size.
     * \param[in] frozen_set The position of the frozen bits in the codeword.
     * \param[in] frozen_set_size The size of the frozen set.
     */
    void compute(std::vector<uint8_t*> node_type,
                 const uint16_t*       frozen_set,
                 const uint16_t        code_size_log,
                 const uint16_t        frozen_set_size);
  };

  srsvec::aligned_vec<int8_t>    llr_alloc;
  std::vector<int8_t*>           llr0;          /*!< \brief Pointers to the upper half of LLRs values at all stages. */
  std::vector<int8_t*>           llr1;          /*!< \brief Pointers to the lower half of LLRs values at all stages. */
  srsvec::aligned_vec<uint8_t>   est_bit;       /*!< \brief Pointers to the temporary estimated bits. */
  params_s                       param;         /*!< \brief Pointer to a Params structure. */
  state_s                        state;         /*!< \brief Pointer to a State. */
  tmp_node_s                     tmp_node_type; /*!< \brief Pointer to a Tmp_node_type. */
  std::unique_ptr<polar_encoder> enc;           /*!< \brief Pointer to a srsran_polar_encoder_t. */

  void init(span<const int8_t> input_llr,
            bit_buffer         data_decoded,
            const uint8_t      code_size_log,
            const uint16_t*    frozen_set,
            const uint16_t     frozen_set_size);

  /*!
   * All decoded bits below a ::RATE_0 node are 0. The function updates the \a p->state->active_node_per_stage
   * pointer to point to the next active node. It is assumed that message bits are initialized to 0.
   *
   */
  void rate_0_node();

  /*!
   * ::RATE_1 nodes at stage \f$ s \f$ return the associated \f$2^s\f$ estimated bits by
   * making a hard decision on them.
   * ::RATE_1 nodes also update message bits vector.
   *
   */
  void rate_1_node(bit_buffer data_decoded);

  /*!
   * ::RATE_R nodes at stage \f$ s \f$ return the associated \f$2^s\f$ decoded bit by calling
   * the child nodes to the right and left of the decoding tree and then polar encoding (xor) their output.
   * At stage \f$ s \f$, this function runs function srsran_vec_function_f_fff() and srsran_vec_function_g_bfff()
   * with vector size \f$2^{ s - 1}\f$ and updates \a llr0 and \a llr1 memory space for stage \f$(s - 1)\f$.
   * This function also runs srsran_vec_xor_bbb() with vector size \f$2^{s-1}\f$ and
   * updates \a estbits memory space for stage \f$(s + 1)\f$.
   *
   */
  void rate_r_node(bit_buffer data_decoded);

  /*!
   * Switches between the different types of node (::RATE_1, ::RATE_0, ::RATE_R) for the SSC algorithm.
   * Nodes in the decoding tree at stage \f$ s\f$ get the \f$2^s\f$ LLRs from the parent node and
   * return the associated \f$2^s\f$ estimated bits.
   *
   */
  void simplified_node(bit_buffer data_decoded);

public:
  polar_decoder_impl(std::unique_ptr<polar_encoder>& enc_, uint8_t nMax);
  ~polar_decoder_impl() = default;
  void decode(span<const int8_t>   input_llr,
              bit_buffer           data_decoded,
              const uint8_t        code_size_log,
              span<const uint16_t> frozen_set,
              span<const uint16_t> frozen_set_size) override;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_POLAR_DECODER_IMPL_H
