// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef STREAM_SERIALIZER_HPP
#define STREAM_SERIALIZER_HPP

#include <stream/detail/util.hpp>
#include <stream/detail/serializer_promise.hpp>
#include <span>
#include <string_view>

#include <coroutine>

namespace stream
{

struct serializer
{
  using promise_type = detail::serializer_promise;

  bool done() const;
  std::string_view read_some(std::span<char> buffer);

 private:

  std::span<char> buffer_;
  std::size_t written_;
  std::string_view remaining_;

  friend detail::serializer_promise;
  detail::unique_handle<promise_type> impl_;
};

}

#endif //STREAM_SERIALIZER_HPP
