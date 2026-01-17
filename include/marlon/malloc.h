#ifndef MARLON_THROWING_MALLOC_H
#define MARLON_THROWING_MALLOC_H

#include <cstdlib>
#include <exception>

namespace marlon
{
  struct Malloc_error: std::exception
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

  struct Malloc_result_guard
  {
    constexpr explicit Malloc_result_guard(void *ptr) noexcept : _ptr{ptr} {}

    ~Malloc_result_guard()
    {
      free(_ptr);
    }

    constexpr auto get() const noexcept -> void *
    {
      return _ptr;
    };

    constexpr auto release() noexcept -> void *
    {
      auto const temp = _ptr;
      _ptr = nullptr;
      return temp;
    };

  private:
    void *_ptr;
  };

  inline auto guarded_malloc(std::size_t size) -> Malloc_result_guard
  {
    return Malloc_result_guard{malloc(size)};
  }
} // namespace marlon

#endif
