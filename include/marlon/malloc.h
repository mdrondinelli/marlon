#ifndef MARLON_THROWING_MALLOC_H
#define MARLON_THROWING_MALLOC_H

#include <cstdlib>
#include <exception>

namespace marlon
{
  struct Malloc_error : std::exception 
  {
  };

  inline auto malloc(std::size_t size) -> void *
  {
    auto const result = std::malloc(size);
    if (!result)
    {
      throw Malloc_error{};
    }
    return result;
  }
  
  inline auto free(void *ptr) noexcept -> void
  {
    std::free(ptr);
  }
}

#endif
