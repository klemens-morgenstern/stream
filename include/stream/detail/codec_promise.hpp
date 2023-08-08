// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef STREAM_DETAIL_CODEC_PROMISE_HPP
#define STREAM_DETAIL_CODEC_PROMISE_HPP

#include <stream/detail/memory_resource_base.hpp>

#include <coroutine>
#include <string_view>

namespace stream { struct codec; }

namespace stream::detail
{

struct codec_promise : detail::memory_resource_base
{
  constexpr static std::suspend_always initial_suspend() noexcept {return {};}

  struct final_suspend_awaitable
  {
    detail::unique_handle<codec_promise> awaited_from;
    bool await_ready() const noexcept {return false;}
    std::coroutine_handle<void> await_suspend(std::coroutine_handle<codec_promise> h) noexcept
    {
      if (!awaited_from)
        return std::noop_coroutine();
#if _MSC_VER
      awaited_from.promise().awaited_from = detail::unique_handle<codec_promise>::from_promise(h.promise());
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

  codec get_return_object();

  void return_void() {}

  void unhandled_exception() { throw;  }

  struct conditional_suspend
  {
    bool written;
    codec::buffer buf;
    bool await_ready()
    {
      [[likely]]
      if (written)
        return true;
      else
        return false;
    }
    void await_suspend(std::coroutine_handle<codec_promise>) {}
    codec::buffer await_resume()
    {
      return buf;
    }
  };

  conditional_suspend yield_value(const char & c);
  conditional_suspend yield_value(std::string_view c);

  conditional_suspend await_transform(const codec::input_t &);

  struct inner_awaitable
  {
    detail::unique_handle<codec_promise> awaited_from;

    constexpr bool await_ready() const noexcept {return false;}
    std::coroutine_handle<codec_promise>
        await_suspend(std::coroutine_handle<codec_promise> h)
    {
      return std::move(awaited_from).release();
    }
    constexpr void await_resume() const noexcept {}
  };

  inner_awaitable await_transform(codec && inner);

  codec * ser;
  detail::unique_handle<codec_promise> awaited_from;

};

}

#endif //STREAM_DETAIL_CODEC_PROMISE_HPP
