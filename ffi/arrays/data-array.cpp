#include <limits>
#include <cassert>
#include <algorithm>
#include "purescript.h"

// Tested with package v5.0.0

FOREIGN_BEGIN( Data_Array )

exports["range"] = [](const boxed& start_) -> boxed {
    const auto start = unbox<int>(start_);
    return [=](const boxed& end_) -> boxed {
        const auto end = unbox<int>(end_);
        const auto step = start > end ? -1 : 1;
        array_t ns;
        for (auto i = start; i != end; i += step) {
            ns.emplace_back(i);
        }
        ns.emplace_back(end);
        return ns;
    };
};

exports["length"] = [](const boxed& xs) -> boxed {
    const long long len = unbox<array_t>(xs).size();
    assert(len <= std::numeric_limits<int>::max());
    return static_cast<int>(len);
};

exports["slice"] = [](const boxed& start_) -> boxed {
    const auto start = unbox<int>(start_);
    return [=](const boxed& end_) -> boxed {
        const auto end = unbox<int>(end_);
        return [=](const boxed& xs_) -> boxed {
            const auto& xs = unbox<array_t>(xs_);
            const long long size = xs.size();
            assert(size <= std::numeric_limits<decltype(start)>::max());
            const auto length = static_cast<decltype(start)>(size);
            const auto _start = std::min(start, length);
            const auto _end   = std::min(end, length);
            return array_t(xs.cbegin() + _start, xs.cbegin() + _end);
        };
    };
};

exports["unsafeIndexImpl"] = [](const boxed& xs) -> boxed {
    return [=](const boxed& n) -> boxed {
        return unbox<array_t>(xs)[unbox<int>(n)];
    };
};

// lettier
exports["findIndexImpl"] = [](const boxed& just) -> boxed {
  return [=](const boxed& nothing) -> boxed {
    return [=](const boxed& f) -> boxed {
      return [=](const boxed& xs_) -> boxed {
        const auto& xs = unbox<array_t>(xs_);
        const long long size = xs.size();

        for (int i = 0; i < size; ++i) {
          if (f(xs[i])) return just(i);
        }
        return nothing;
      };
    };
  };
};

// lettier
exports["_deleteAt"] = [](const boxed& just) -> boxed {
  return [=](const boxed& nothing) -> boxed {
    return [=](const boxed& i_) -> boxed {
      return [=](const boxed& l_) -> boxed {
        const auto i = unbox<int>(i_);
        const auto& l = unbox<array_t>(l_);
        const long long size = l.size();
        auto xs = std::deque<boxed>();

        if (i < 0 || i >= size) return nothing;

        for (int j = 0; j < size; ++j) {
          if (j == i) continue;
          xs.push_back(l[j]);
        }
        return just(xs);
      };
    };
  };
};

FOREIGN_END
