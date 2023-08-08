// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <stream/codec.hpp>

#include <vector>

#include "doctest.h"

stream::codec hex_to_bin()
{
  while (true)
  {
    auto in = co_await stream::codec::input;
    switch (in->front())
    {
      case '0': in = co_yield "0000"; break;
      case '1': in = co_yield "0001"; break;
      case '2': in = co_yield "0010"; break;
      case '3': in = co_yield "0011"; break;
      case '4': in = co_yield "0100"; break;
      case '5': in = co_yield "0101"; break;
      case '6': in = co_yield "0110"; break;
      case '7': in = co_yield "0111"; break;
      case '8': in = co_yield "1000"; break;
      case '9': in = co_yield "1001"; break;
      case 'a': case 'A': co_yield "1010"; break;
      case 'b': case 'B': co_yield "1011"; break;
      case 'c': case 'C': co_yield "1100"; break;
      case 'd': case 'D': co_yield "1101"; break;
      case 'e': case 'E': co_yield "1110"; break;
      case 'f': case 'F': co_yield "1111"; break;
      default:
        throw std::invalid_argument("invalid input: " + std::string(in.get()));
    }
    in.consume(1u);
  }
}

TEST_CASE("hex_to_bin-codec")
{
  auto c = hex_to_bin();
  std::string buffer;
  buffer.resize(12);

  using std::operator""sv;

  auto res = c.encode_some("ABCDEF", buffer);
  CHECK(res.consumed == 3u);
  CHECK(res.written == "1010" "1011" "1100");
}