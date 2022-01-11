/*************************************************************************
 * Copyright © 2022 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of http_utils.
 * Distributed under the MIT License.
 * See accompanying file LICENSE (at the root of this repository)
 *************************************************************************/

#include "request_generator.hpp"

#include "uri_parser.hpp"

using http_utils::request_generator;

request_generator::request_generator()
    : request_generator(std::pmr::get_default_resource())
{
}

request_generator::request_generator(std::pmr::memory_resource *mem)
    : mem(mem)
    , headers(mem)
    , head(mem)
{
	if(!mem) throw std::runtime_error("cannot create request generator without memory resource");
}

request_generator& request_generator::uri(std::string_view u)
{
	uri_parser prs(mem, u);
	head = "GET ";
	head += prs.request();
	head += " HTTP/1.1\r\nHost:";
	head += prs.host();
	head += "\r\n";
	return *this;
}

request_generator& request_generator::header(std::string_view name, std::string_view val)
{
	headers +=
	        std::pmr::string(name, mem)
	      + ':'
	      + std::pmr::string(val, mem)
	      + std::pmr::string("\r\n", mem)
	        ;
	return *this;
}

std::pmr::string request_generator::body(std::string_view cnt) const
{
	if(cnt.size() == 0) return head + headers + std::pmr::string("\r\n", mem);
	std::array<char, std::numeric_limits<std::size_t>::digits10 + 1> str;
	auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), cnt.size());
	assert(ec == std::errc());
	std::string_view len(str.data(), ptr);
	std::pmr::string ret(head, mem);
	ret +=
	        headers
	      + std::pmr::string("Content-Length:", mem)
	      + std::pmr::string(len, mem)
	      + std::pmr::string("\r\n\r\n", mem)
	      + std::pmr::string(cnt, mem)
	      + std::pmr::string("\r\n", mem)
	      ;
	return ret;
}
