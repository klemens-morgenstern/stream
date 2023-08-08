//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STREAM_DETAIL_MEMORY_RESOURCE_BASE_HPP
#define STREAM_DETAIL_MEMORY_RESOURCE_BASE_HPP

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory_resource>

namespace stream::detail
{

struct memory_resource_base
{
  /// Allocate the memory and put the allocator behind the async memory
  template<typename ... Args>
  void *operator new(const std::size_t size, Args & ... args)
  {
    std::pmr::memory_resource * resource = nullptr;
    constexpr std::array<bool, sizeof...(Args)> is_res{std::is_same_v<Args, std::pmr::memory_resource*>...};
    constexpr auto pos = std::find(is_res.cbegin(), is_res.cend(), true) - is_res.cbegin();
    if constexpr (pos == sizeof...(Args))
      resource = std::pmr::get_default_resource();
    else
      resource = std::get<pos>(std::tie(args...));

    auto data = resource->allocate(size + sizeof(resource));
    unsigned char *buf = static_cast<unsigned char*>(data);
    auto ptr = std::next(buf, size);
    std::memcpy(ptr, &resource, sizeof(std::pmr::memory_resource*));
    *reinterpret_cast<std::pmr::memory_resource**>(std::next(buf, size)) = resource;
    return data;
  }


  /// Deallocate the memory and destroy the allocator in the async memory.
  void operator delete(void *raw_, const std::size_t size)
  {
    const auto raw = static_cast<unsigned char *>(raw_);
    std::pmr::memory_resource * res;
    std::memcpy(&res, std::next(raw, size), sizeof(std::pmr::memory_resource*));
    res->deallocate(raw_, size + sizeof(std::pmr::memory_resource*));
  }
};

}

#endif //STREAM_DETAIL_MEMORY_RESOURCE_BASE_HPP
