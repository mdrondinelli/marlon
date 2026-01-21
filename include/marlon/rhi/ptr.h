#ifndef MARLON_RHI_PTR_H
#define MARLON_RHI_PTR_H

#include <concepts>
#include <utility>

namespace marlon::rhi
{
  struct Object;

  template <std::derived_from<Object> T>
  struct Ptr;

  template <std::derived_from<Object> T>
  constexpr auto move_ptr(T *p) noexcept -> Ptr<T>;

  template <std::derived_from<Object> T>
  auto copy_ptr(T *p) noexcept -> Ptr<T>;

  template <typename T>
  struct Is_ptr;

  template <typename T>
  constexpr auto Is_ptr_v = Is_ptr<T>::value;

  template <std::derived_from<Object> T>
  struct Ptr
  {
    using Value_type = T;

    constexpr Ptr() noexcept = default;

    constexpr Ptr(std::nullptr_t) noexcept
    {
    }

    ~Ptr()
    {
      if (_p)
      {
        _p->release();
      }
    }

    Ptr(Ptr const &other) noexcept
        : _p{other._p}
    {
      if (_p)
      {
        _p->acquire();
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

    template <typename U>
      requires std::derived_from<T, U>
    constexpr operator Ptr<U>() && noexcept
    {
      return move_ptr(static_cast<U *>(_p));
    }

    template <typename U>
      requires std::derived_from<T, U>
    operator Ptr<U>() const noexcept
    {
      return copy_ptr(static_cast<U *>(_p));
    }

    template <typename U>
      requires Is_ptr_v<U>
    constexpr explicit operator U() && noexcept
    {
      auto const p = _p;
      _p = nullptr;
      return move_ptr(static_cast<U::Value_type *>(p));
    }

    template <typename U>
      requires Is_ptr_v<U>
    explicit operator U() const noexcept
    {
      return copy_ptr(static_cast<U::Value_type *>(_p));
    }

  private:
    struct Move_tag
    {
    };

    struct Copy_tag
    {
    };

    friend constexpr auto move_ptr<T>(T *p) noexcept -> Ptr<T>;

    friend auto copy_ptr<T>(T *p) noexcept -> Ptr<T>;

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
  struct Is_ptr: std::false_type
  {
  };

  template <typename T>
  struct Is_ptr<Ptr<T>>: std::true_type
  {
  };

  template <std::derived_from<Object> T>
  constexpr auto move_ptr(T *p) noexcept -> Ptr<T>
  {
    return Ptr<T>{p};
  }

  template <std::derived_from<Object> T>
  auto copy_ptr(T *p) noexcept -> Ptr<T>
  {
    p->acquire();
    return Ptr<T>{p};
  }
}

#endif
