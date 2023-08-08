// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <stream/codec.hpp>
#include <algorithm>

namespace stream
{

bool codec::done()  const
{
  return (!impl_ || impl_.done()) && remaining_.empty();
}

auto codec::encode_some(
    std::string_view input,
    std::span<char> buffer) -> result
{
  auto n = (std::min)(remaining_.size(), buffer.size());
  std::copy_n(remaining_.begin(), n, buffer.begin());
  written_ = n;
  remaining_.remove_prefix(n);
  buffer_ = buffer;
  read_ = 0u;
  input_ = input;

  if (!done() && (buffer_.size() != written_)) // space remaining
  {
    impl_.promise().ser = this;
    impl_.resume();
    impl_.promise().ser = nullptr;

  }
  return {read_, {buffer.data(), written_}};
}

namespace detail
{

codec codec_promise::get_return_object()
{
  codec s;
  s.impl_ = detail::unique_handle<codec_promise>::from_promise(*this);
  return s;
}

auto codec_promise::yield_value(const char & c) -> conditional_suspend
{
  if (ser->written_ < ser->buffer_.size())
  {
    ser->buffer_[ser->written_] = c;
    ser->written_++;
    return conditional_suspend{true, codec::buffer{ser}};
  }
  else
    ser->remaining_ = {&c, 1u};

  return conditional_suspend{false, codec::buffer{ser}};
}

auto codec_promise::yield_value(std::string_view c) -> conditional_suspend
{
  if (ser->written_ < ser->buffer_.size())
  {

    auto n = (std::min)(c.size(), (ser->buffer_.size() - ser->written_));
    std::copy_n(c.begin(), n, ser->buffer_.begin() + ser->written_);
    ser->written_ += n;

    c.remove_prefix(n);
    if (!c.empty())
      ser->remaining_ = c;

    return conditional_suspend{c.empty(), codec::buffer{ser}};
  }
  else
    ser->remaining_ = c;

  return conditional_suspend{false, codec::buffer{ser}};
}

auto codec_promise::await_transform(const codec::input_t &) -> conditional_suspend
{
  return conditional_suspend{!ser->input_.empty(), codec::buffer{ser}};
}

auto codec_promise::await_transform(codec && inner) -> inner_awaitable
{
  return inner_awaitable{std::move(inner.impl_)};
}


}

}