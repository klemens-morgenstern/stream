// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef STREAM_DETAIL_SERIALIZER_PROMISE_HPP
#define STREAM_DETAIL_SERIALIZER_PROMISE_HPP

#include <coroutine>

#include <string_view>

namespace stream { struct serializer; }

namespace stream::detail
{

struct serializer_promise
{
  constexpr static std::suspend_always initial_suspend() noexcept {return {};}
  constexpr static std::suspend_always final_suspend  () noexcept {return {};}

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

  serializer * ser;
};

}

#endif //STREAM_DETAIL_SERIALIZER_PROMISE_HPP
