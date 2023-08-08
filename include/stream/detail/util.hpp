// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef STREAM_UTIL_HPP
#define STREAM_UTIL_HPP

#include <coroutine>
#include <memory>

namespace stream::detail
{

template<typename T>
struct unique_handle
{
  unique_handle() noexcept = default;
  unique_handle(std::nullptr_t) noexcept {}

  std::coroutine_handle<T> release()
  {
    return std::coroutine_handle<T>::from_promise(*handle_.release());
  }

  void* address() const noexcept { return get_handle_().address(); }
  static unique_handle from_address(void* a) noexcept
  {
    unique_handle res;
    res.handle_.reset(&std::coroutine_handle<T>::from_address(a).promise());
    return res;
  }

  bool done() const noexcept { return get_handle_().done(); }
  void operator()() const { resume(); }
  operator bool() const { return static_cast<bool>(handle_); }
  void resume() const { get_handle_().resume(); }
  void destroy() { handle_.reset(); }

  T & promise() {return *handle_;}
  constexpr static unique_handle from_promise(T &p) noexcept
  {
    unique_handle res;
    res.handle_.reset(&p);
    return res;
  }


 private:
  struct deleter_
  {
    void operator()(T * p)
    {
      std::coroutine_handle<T>::from_promise(*p).destroy();
    }
  };

  std::coroutine_handle<T> get_handle_() const
  {
    return std::coroutine_handle<T>::from_promise(*handle_);
  }


  std::unique_ptr<T, deleter_> handle_;

};

template<>
struct unique_handle<void>
{
  unique_handle() noexcept = default;
  unique_handle(std::nullptr_t) noexcept {}

  std::coroutine_handle<void> release()
  {
    return std::coroutine_handle<void>::from_address(handle_.release());
  }
  void* address() const noexcept { return get_handle_().address(); }
  static unique_handle<void> from_address(void* a) noexcept
  {

    unique_handle res;
    res.handle_.reset(std::coroutine_handle<void>::from_address(a).address());
    return res;
  }

  bool done() const noexcept { return get_handle_().done(); }
  void operator()() const { resume(); }
  void resume() const { get_handle_().resume(); }
  void destroy() { handle_.reset(); }

 private:
  struct deleter_
  {
    void operator()(void * p)
    {
      std::coroutine_handle<void>::from_address(p).destroy();
    }
  };

  std::coroutine_handle<void> get_handle_() const
  {
    return std::coroutine_handle<void>::from_address(handle_.get());
  }

  std::unique_ptr<void, deleter_> handle_;
};

}

#endif //STREAM_UTIL_HPP
