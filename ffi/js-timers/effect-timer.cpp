#include <thread>

#include "purescript.h"

FOREIGN_BEGIN( Effect_Timer )

// lettier
exports["setInterval"] = [](const boxed& ms_) -> boxed {
  return [=](const boxed& fn) -> boxed {
    return [=]() -> boxed {
      const auto ms = unbox<int>(ms_);
      auto id = std::make_shared<bool>(true);
      auto t = std::thread([=](){
        while (*id) {
          std::this_thread::sleep_for(std::chrono::milliseconds(ms));
          if (*id) { fn(); }
        }
      });
      t.detach();
      return box<std::shared_ptr<bool>>(id);
    };
  };
};

// lettier
exports["setTimeout"] = [](const boxed& ms_) -> boxed {
  return [=](const boxed& fn) -> boxed {
    return [=]() -> boxed {
      const auto ms = unbox<int>(ms_);
      auto id = std::make_shared<bool>(true);
      auto t = std::thread([=](){
        if (!*id) { return; }
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        if (*id) { fn(); }
      });
      t.detach();
      return box<std::shared_ptr<bool>>(id);
    };
  };
};

// lettier
exports["clearInterval"] = [](const boxed& id_) -> boxed {
  return [=]() -> boxed {
    const auto& id = unbox<std::shared_ptr<bool>>(id_);
    *id = false;
    return boxed();
  };
};

FOREIGN_END
