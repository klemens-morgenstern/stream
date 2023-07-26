// Copyright (c) 2023 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <stream/serializer.hpp>

#include <vector>

#include "doctest.h"

stream::serializer serialize_ints(std::vector<int> data)
{
  for (auto idx = 0u; idx < data.size(); idx++)
  {
    if (idx != 0)
      co_yield ", ";
    co_yield std::to_string(data[idx]);
  }
}

TEST_CASE("simple")
{
  auto s = serialize_ints({1,2,3,4,5,6,7,8,9,10});
  std::string buffer;
  buffer.resize(10);

  using std::operator""sv;

  CHECK(s.read_some(buffer) == "1, 2, 3, 4"sv);
  CHECK(s.read_some(buffer) == ", 5, 6, 7,"sv);
  CHECK(s.read_some(buffer) == " 8, 9, 10"sv);
}