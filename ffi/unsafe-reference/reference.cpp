#include <iostream>

#include "purescript.h"

FOREIGN_BEGIN( Unsafe_Reference )

// lettier
exports["reallyUnsafeRefEq"] = [](const boxed& a_) -> boxed {
  std::cout << "TODO: reallyUnsafeRefEq" << "\n";
  return [=](const boxed& b_) -> boxed {
    auto const a = unbox<double>(a_);
    auto const b = unbox<double>(b_);
    return a == b;
  };
};

FOREIGN_END
