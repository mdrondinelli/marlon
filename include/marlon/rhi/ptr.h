#ifndef MARLON_RHI_PTR_H
#define MARLON_RHI_PTR_H

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  namespace detail
  {
    auto get_object(Object *p) noexcept -> Object *
    {
      return p;
    }

    auto get_object(Interface *p) noexcept -> Object *;

    auto get_object(Descriptor_set_layout *p) noexcept -> Object *;

    auto get_object(Descriptor_set *p) noexcept -> Object *;

    auto get_object(Pipeline_layout *p) noexcept -> Object *;

    auto get_object(Compute_pipeline *p) noexcept -> Object *;

    auto get_object(Graphics_pipeline *p) noexcept -> Object *;

    auto get_object(Buffer *p) noexcept -> Object *;

    auto get_object(Image *p) noexcept -> Object *;

    auto get_object(Surface *p) noexcept -> Object *;

    auto get_object(Swapchain *p) noexcept -> Object *;

    auto get_object(Command_buffer *p) noexcept -> Object *;
  } // namespace detail

  template <typename T>
  struct Ptr;

  template <typename T>
  constexpr auto capture(T *&&p) -> Ptr<T>;

  template <typename T>
  struct Ptr
  {
    constexpr Ptr() noexcept = default;

    constexpr Ptr(std::nullptr_t) noexcept
    {
    }

    ~Ptr()
    {
      if (_p)
      {
        release_object(get_object(_p));
      }
    }

    Ptr(Ptr const &other) noexcept
        : _p{other._p}
    {
      if (_p)
      {
        acquire_object(get_object(_p));
      }
    }

    Ptr &operator=(Ptr const &other) noexcept
    {
      auto temp{other};
      swap(temp);
      return *this;
    }

    constexpr Ptr(Ptr &&other) noexcept
        : _p{std::exchange(other._p, nullptr)}
    {
    }

    constexpr Ptr &operator=(Ptr &&other) noexcept
    {
      auto temp{std::move(other)};
      swap(temp);
      return *this;
    }

    constexpr auto operator*() const noexcept -> T &
    {
      return *_p;
    }

    constexpr auto operator->() const noexcept -> T *
    {
      return _p;
    }

    constexpr auto get() const noexcept -> T *
    {
      return _p;
    }

    constexpr auto release() noexcept -> T *
    {
      auto temp{_p};
      _p = nullptr;
      return temp;
    }

  private:
    friend auto capture<T>(T *&&p) -> Ptr<T>;

    constexpr explicit Ptr(T *p) noexcept
        : _p{p}
    {
    }

    constexpr auto swap(Ptr &other) noexcept -> void
    {
      std::swap(_p, other._p);
    }

    T *_p{};
  };

  template <typename T>
  constexpr auto capture(T *&&p) -> Ptr<T>
  {
    return Ptr<T>{p};
  }
} // namespace marlon::rhi

#endif
