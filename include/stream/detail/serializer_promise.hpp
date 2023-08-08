// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef STREAM_DETAIL_SERIALIZER_PROMISE_HPP
#define STREAM_DETAIL_SERIALIZER_PROMISE_HPP

#include <stream/detail/memory_resource_base.hpp>

#include <coroutine>
#include <string_view>

namespace stream { struct serializer; }

namespace stream::detail
{

struct serializer_promise : detail::memory_resource_base
{
  constexpr static std::suspend_always initial_suspend() noexcept {return {};}

  struct final_suspend_awaitable
  {
    detail::unique_handle<serializer_promise> awaited_from;
    bool await_ready() const noexcept {return false;}
    std::coroutine_handle<void> await_suspend(std::coroutine_handle<serializer_promise> h) noexcept
    {
      if (!awaited_from)
        return std::noop_coroutine();
#if _MSC_VER
      awaited_from.promise().awaited_from = detail::unique_handle<serializer_promise>::from_promise(h.promise());
#else
      h.destroy();
#endif
      return std::move(awaited_from).release();
    }
    void await_resume() const noexcept {}
  };

  final_suspend_awaitable final_suspend  () noexcept
  {
    return {std::move(awaited_from)};
  }

  serializer get_return_object();

  void return_void() {}

  void unhandled_exception() { throw;  }

  struct conditional_suspend
  {
    bool written;
    bool await_ready()
    {
      [[likely]]
      if (written)
        return true;
      else
        return false;
    }
    void await_suspend(std::coroutine_handle<serializer_promise>) {}
    void await_resume() {}
  };

  conditional_suspend yield_value(const char & c);
  conditional_suspend yield_value(std::string_view c);

  struct yield_value_awaitable
  {
    detail::unique_handle<serializer_promise> awaited_from;

    constexpr bool await_ready() const noexcept {return false;}
    std::coroutine_handle<serializer_promise>
        await_suspend(std::coroutine_handle<serializer_promise> h)
    {
      return std::move(awaited_from).release();
    }
    constexpr void await_resume() const noexcept {}
  };

  yield_value_awaitable yield_value(serializer && inner);

  serializer * ser;
  detail::unique_handle<serializer_promise> awaited_from;

};

}

#endif //STREAM_DETAIL_SERIALIZER_PROMISE_HPP
