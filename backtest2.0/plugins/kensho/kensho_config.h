#ifndef INCLUDE_KENSHO_CONFIG_H
#define INCLUDE_KENSHO_CONFIG_H
#include <string>
namespace kensho_logic {
  // attention  first init, then readonly
  class KenshoConfig {
   public:
    void set_yield_url(std::string url) {
      yield_url_ = url;
    }

    std::string yield_url() const{
      return yield_url_;
    }

   private:
    std::string yield_url_;

  };


  KenshoConfig &GetKenshoConfig(); 
}

#endif
