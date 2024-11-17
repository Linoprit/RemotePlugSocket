#ifndef BH1750_HELPER_H
#define BH1750_HELPER_H

#pragma once

#include <BH1750.h>

namespace msmnt {

class BH1750Helper {
public:
  BH1750Helper();
  virtual ~BH1750Helper(){};

  void InitHardware();
  void Cycle();

private:
    BH1750* _lightMeter;

};
} // namespace msmnt

#endif
