#ifndef MARLON_CONCEPTS_H
#define MARLON_CONCEPTS_H

#include <type_traits>

namespace marlon
{
  template <typename T>
  concept Pointer = std::is_pointer_v<T>;
}

#endif
