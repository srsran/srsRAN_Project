#include "srsran/rohc/rohc_factory.h"
#ifdef ENABLE_ROHC_LIB
#include "rohc_lib/rohc_lib_engine.h"
#endif

using namespace srsran;
using namespace srsran::rohc;

std::unique_ptr<rohc_engine> srsran::rohc::create_rohc_engine()
{
#ifdef ENABLE_ROHC_LIB
  return std::make_unique<rohc_lib_engine>();
#else
  return nullptr;
#endif
}
