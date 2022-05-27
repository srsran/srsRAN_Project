
#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_PROCESSOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_PROCESSOR_IMPL_H

#include "srsgnb/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"

namespace srsgnb {

/// Groups the necessary parameters to construct a generic PDSCH processor.
struct pdsch_processor_configuration {
  /// Provides a PDSCH encoder instance. Ownership is transferred to the processor.
  std::unique_ptr<pdsch_encoder> encoder;
  /// Provides a PDSCH modulator instance. Ownership is transferred to the processor.
  std::unique_ptr<pdsch_modulator> modulator;
  /// Provides a DMRS for PDSCH processor. Ownership is transferred to the processor.
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
};

/// Describes a generic PDSCH processor.
class pdsch_processor_impl : public pdsch_processor
{
private:
  std::unique_ptr<pdsch_encoder>                                                  encoder;
  std::unique_ptr<pdsch_modulator>                                                modulator;
  std::unique_ptr<dmrs_pdsch_processor>                                           dmrs;
  std::array<static_vector<uint8_t, MAX_CODEWORD_SIZE>, MAX_NOF_TRANSPORT_BLOCKS> codewords;

  /// \brief Computes the number of RE used for mapping PDSCH data.
  ///
  /// The number of RE excludes the elements described by \c pdu reserved and the RE used for DMRS.
  ///
  /// \param[in] pdu Describes a PDSCH transmission.
  /// \return The number of resource elements.
  static unsigned compute_nof_data_re(const pdu_t& pdu);

  /// \brief Encodes a codeword as per TS 38.212 section 7.2.
  ///
  /// \param[in] data        Provides the data to transmit as packed bits.
  /// \param[in] codeword_id Indicates the codeword identifier.
  /// \param[in] nof_layers  Indicates the number of layers the codeword is mapped on to.
  /// \param[in] Nre         Indicates the number of resource elements used for PDSCH mapping.
  /// \param[in] pdu         Provides the PDSCH processor PDU.
  /// \return A view of the encoded codeword.
  span<const uint8_t>
  encode(span<const uint8_t> data, unsigned codeword_id, unsigned nof_layers, unsigned Nre, const pdu_t& pdu);

  /// \brief Modulates a PDSCH transmission as per TS 38.211 section 7.3.1.
  /// \param[out] grid          Provides the destination resource grid.
  /// \param[in] temp_codewords Provides the encoded codewords.
  /// \param[in] pdu            Provides the PDSCH processor PDU.
  void modulate(resource_grid_writer& grid, span<const span<const uint8_t> > temp_codewords, const pdu_t& pdu);

  /// \brief Generates and maps DMRS for the PDSCH transmission as per TS 38.211 section 7.4.1.1.
  /// \param[out] grid   Provides the destination resource grid.
  /// \param[in] pdu     Provides the PDSCH processor PDU.
  void put_dmrs(resource_grid_writer& grid, const pdu_t& pdu);

public:
  /// \brief Creates a generic PDSCH processor.
  /// \param[in] config Provides the necessary configuration.
  pdsch_processor_impl(pdsch_processor_configuration& config) :
    encoder(std::move(config.encoder)), modulator(std::move(config.modulator)), dmrs(std::move(config.dmrs))
  {
    srsran_assert(encoder != nullptr, "Invalid encoder pointer.");
    srsran_assert(modulator != nullptr, "Invalid modulator pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                        grid,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override;
};

std::unique_ptr<pdsch_processor> create_pdsch_processor(pdsch_processor_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_PROCESSOR_IMPL_H
