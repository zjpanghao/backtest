#include "kensho/kensho_config.h"

namespace kensho_logic {
  KenshoConfig &GetKenshoConfig() {
    static KenshoConfig config;
    return config;
  }
}
