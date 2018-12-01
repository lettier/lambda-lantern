#include <chrono>

#include "purescript.h"

FOREIGN_BEGIN( Effect_Now )

exports["now"] = []() -> boxed {
  double m =
      std::chrono::system_clock::now().time_since_epoch()
    / std::chrono::milliseconds(1);
  return box<double>(m);
};

FOREIGN_END
