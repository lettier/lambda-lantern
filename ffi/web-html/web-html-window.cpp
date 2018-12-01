#include <iostream>
#include <thread>

#include "purescript.h"

FOREIGN_BEGIN( Web_HTML_Window )

// lettier
exports["_requestAnimationFrame"] = [](const boxed& fn) -> boxed {
  return [=](const boxed& w_) -> boxed {
    return [=]() -> boxed {
      auto id = std::make_shared<bool>(true);
      auto t = std::thread([=](){
        if (!*id) { return; }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        if (*id) { fn(); }
      });
      t.detach();
      return box<std::shared_ptr<bool>>(id);
    };
  };
};

FOREIGN_END
