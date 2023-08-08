// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef STREAM_CODEC_HPP
#define STREAM_CODEC_HPP

#include <stream/detail/util.hpp>


#include <cassert>
#include <coroutine>
#include <span>
#include <string_view>


namespace stream
{
namespace detail { struct codec_promise; }

struct codec
{
  using promise_type = detail::codec_promise;

  struct result
  {
    std::size_t consumed;
    std::string_view written;
  };
  struct buffer
  {
    const std::string_view & get()        const {assert(codec_ != nullptr); return  codec_->input_;}
    const std::string_view & operator*()  const {assert(codec_ != nullptr); return  codec_->input_;}
    const std::string_view * operator->() const {assert(codec_ != nullptr); return &codec_->input_;}
    void consume(std::size_t n)
    {
      codec_->read_ += n;
      codec_->input_.remove_prefix(n);
    }

   private:
    explicit buffer(codec * codec_) : codec_(codec_) {}
    codec * codec_;
    friend detail::codec_promise;
  };

  struct input_t {};
  constexpr static input_t input{};

  bool done() const;
  result encode_some(std::string_view input_buffer,
                     std::span<char> output_buffer);


 private:

  std::string_view input_;
  std::size_t read_;

  std::span<char> buffer_;
  std::size_t written_;
  std::string_view remaining_;

  friend detail::codec_promise;
  detail::unique_handle<promise_type> impl_;
};

}

#include <stream/detail/codec_promise.hpp>


#endif //STREAM_CODEC_HPP
