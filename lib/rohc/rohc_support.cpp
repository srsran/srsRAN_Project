#include "srsran/rohc/rohc_support.h"

using namespace srsran;
using namespace srsran::rohc;

bool srsran::rohc::rohc_supported()
{
#ifdef ENABLE_ROHC_LIB
  return true;
#else
  return false;
#endif
}
