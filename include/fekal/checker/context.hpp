// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/checker/scope.hpp>
#include <fekal/ast.hpp>
#include <unordered_map>

namespace fekal {

struct Context
{
    unsigned scopeIndex = 0;

    void reset()
    {
        scopeIndex = 0;
        nodeScopes.clear();
        scopes.clear();
        scopes.push_back(std::make_shared<Scope>());
    }

    Scope& get_scope()
    {
        return *scopes.at(scopeIndex);
    }

    Scope& global_scope()
    {
        return *scopes.at(0);
    }

    template<class Node>
    Scope& push_scope(const Node& node)
    {
        auto scope = std::make_shared<Scope>();
        scopes.emplace_back(scope);
        nodeScopes[&node] = scope;
        scopeIndex++;
        return *scopes.back();
    }

    template<class Node>
    Scope& get_scope_by_node(const Node& node)
    {
        return *nodeScopes.at(&node);
    }

    bool has_symbol(const std::string symbol)
    {
        for (int i = scopeIndex; i >= 0; i--) {
            if (scopes.at(i)->has_symbol(symbol)) {
                return true;
            }
        }
        return false;
    }

    void pop_scope()
    {
        if (scopeIndex == 0) {
            throw std::runtime_error("Not allowed to erase global scope");
        }
        scopes.erase(scopes.begin() + scopeIndex);
        scopeIndex--;
    }

    Scope& peek_scope() const
    {
        return *scopes.at(scopeIndex);
    }

private:
    std::vector<std::shared_ptr<Scope>> scopes {std::make_shared<Scope>()};
    std::unordered_map<const void*, std::shared_ptr<Scope>> nodeScopes {};
};

} // namespace fekal
