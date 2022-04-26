include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding/short)
add_library(short_block STATIC short_block_encoder_impl.cpp)

