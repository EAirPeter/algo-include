// Compressed Pair
#pragma once

#include <utility>

namespace sqwh {

namespace impl {

template<
  class Ty, int kIdx,
  bool kIsEmptyBase = !::std::is_final_v<Ty> && ::std::is_empty_v<Ty>
>
struct CompressedPairBase {

};

}

template<class Ty1, class Ty2>
class CompressedPair {

};

}
