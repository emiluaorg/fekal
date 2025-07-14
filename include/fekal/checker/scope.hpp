// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <algorithm>
#include <fekal/ast.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace fekal {

struct Symbol
{
    Symbol(const std::string& name) : name{name} {}

    std::string name;
    unsigned references = 0;

    inline void increase_reference()
    {
        references++;
    }
};

struct Scope
{
    bool has_symbol(const std::string& id) const
    {
        return symbols.contains(id) || inheritSymbols.contains(id);
    }

    bool declare_symbol(const Symbol& symbol)
    {
        if (!symbols.contains(symbol.name)) {
            auto symbol_ptr = std::make_shared<Symbol>(symbol);
            symbols.insert(std::make_pair(symbol.name, symbol_ptr));
            symbolsOrder.emplace_back(symbol_ptr);
            return true;
        }
        return false;
    }

    bool declare_inherit_symbol(const Symbol& symbol)
    {
        if (!inheritSymbols.contains(symbol.name)) {
            auto symbol_ptr = std::make_shared<Symbol>(symbol);
            inheritSymbols.insert(std::make_pair(symbol.name, symbol_ptr));
            return true;
        }
        return false;
    }

    size_t num_symbols()
    {
        return symbols.size();
    }

    Symbol& get_symbol(const std::string& name)
    {
        if (symbols.contains(name)) {
            return *symbols.at(name).get();
        }
        throw std::runtime_error("Symbol not found");
    }

    unsigned get_symbol_position(const std::string& name)
    {
        auto it = std::find_if(
            symbolsOrder.begin(),
            symbolsOrder.end(),
            [&name](const std::shared_ptr<Symbol>& symbol) {
                return symbol->name == name;
            });
        return std::distance(symbolsOrder.begin(), it);
    }

    void increase_reference(std::string name)
    {
        if (has_symbol(name)) {
            symbols.at(name)->increase_reference();
        }
    }

    private:
        std::vector<std::shared_ptr<Symbol>> symbolsOrder;
        std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
        std::unordered_map<std::string, std::shared_ptr<Symbol>> inheritSymbols;
};

} // namespace fekal
