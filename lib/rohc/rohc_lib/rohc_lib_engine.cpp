#include "rohc_lib_engine.h"
#include "rohc/rohc.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srsran::rohc;

rohc_lib_engine::rohc_lib_engine() : logger(srslog::fetch_basic_logger("ROHC"))
{
  logger.info("Created ROHC engine with ROHC library version {}", rohc_version());
}
