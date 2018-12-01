#include "purescript.h"

FOREIGN_BEGIN( Web_HTML )

// lettier
exports["window"] = []() -> boxed {
  return boxed();
};

FOREIGN_END
