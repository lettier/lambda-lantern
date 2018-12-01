#include "purescript.h"

FOREIGN_BEGIN( Data_Ord_Unsafe )

exports["unsafeCompareImpl"] = [](const boxed& lt) -> boxed {
  return [=](const boxed& eq) -> boxed {
    return [=](const boxed& gt) -> boxed {
      return [=](const boxed& x_) -> boxed {
        const auto x = unbox<double>(x_);
        return [=](const boxed& y_) -> boxed {
          const auto y = unbox<double>(y_);
          return x < y ? lt : x == y ? eq : gt;
        };
      };
    };
  };
};

FOREIGN_END
