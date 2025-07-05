// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <istream>
#include <fstream>
#include <fekal/parser.hpp>
#include <fekal/printer.hpp>
#include <iostream>

static std::string read_file(std::istream& s)
{
    std::string ret;

    std::streampos sz;
    s.seekg(0, std::ios::end);
    sz = s.tellg();
    s.seekg(0, std::ios::beg);

    if (s.rdstate() != std::ios_base::goodbit) {
        throw std::system_error{std::io_errc::stream};
    }

    if (sz == 0)
        return ret;

    ret.resize(sz);
    s.read(ret.data(), sz);

    if (s.rdstate() != std::ios_base::goodbit) {
        throw std::system_error{std::io_errc::stream};
    }

    return ret;
}

int main(int argc, char* argv[])
{
    std::ifstream in{argv[1], std::ios::in | std::ios::binary};
    std::string source = read_file(in);
    try {
        fekal::print(std::cout, fekal::parse(source));
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
