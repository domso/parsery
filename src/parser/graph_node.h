#pragma once

#include <optional>
#include <stdint.h>
#include <memory>
#include <vector>

#include "type_union.h"


template<typename Tnode, typename Tedge>
class graph_node {
public:
    auto degree() const {
        return m_children.size();
    }

    template<typename Tcall>
    void child(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(ptr);
            }
        }
    }

    void remove_edge(const std::shared_ptr<graph_node<Tnode, Tedge>>& c) {
        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            if (it->second == c) {
                m_children.erase(it);
                return;
            }
        }
    }
    
    template<typename Tcall>
        requires std::regular_invocable<Tcall, graph_node<Tnode, Tedge>&, Tedge&, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(*this, edge, ptr);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, graph_node<Tnode, Tedge>&, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(*this, ptr);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, graph_node<Tnode, Tedge>&, Tedge&>
    void edge(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(*this, edge);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, Tedge&, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(edge, ptr);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, Tedge&>
    void edge(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(edge);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(ptr);
            }
        }
    }

    template<typename Tcall>
    void child(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(ptr);
            }
        }
    }
    
    template<typename Tcall>
        requires std::regular_invocable<Tcall, graph_node<Tnode, Tedge>&, Tedge&, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(*this, edge, ptr);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, graph_node<Tnode, Tedge>&, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(*this, ptr);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, graph_node<Tnode, Tedge>&, Tedge&>
    void edge(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(*this, edge);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, Tedge&, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(edge, ptr);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, Tedge&>
    void edge(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(edge);
            }
        }
    }

    template<typename Tcall>
        requires std::regular_invocable<Tcall, std::shared_ptr<graph_node<Tnode, Tedge>>&>
    void edge(const size_t n, const Tcall& call) const {
        if (n < m_children.size()) {
            auto& [edge, ptr] = m_children[n];
            if (ptr != nullptr) {
                call(ptr);
            }
        }
    }

    template<typename Tcall>
    void for_each_child(const Tcall& call) {
        for (size_t i = 0; i < m_children.size(); i++) {
            child(i, call);
        }
    }

    template<typename Tcall>
    void for_each_child(const Tcall& call) const {
        for (size_t i = 0; i < m_children.size(); i++) {
            child(i, call);
        }
    }   
    
    template<typename Tcall>
    void for_each_edge(const Tcall& call) {
        for (size_t i = 0; i < m_children.size(); i++) {
            edge(i, call);
        }
    }

    template<typename Tcall>
    void for_each_edge(const Tcall& call) const {
        for (size_t i = 0; i < m_children.size(); i++) {
            edge(i, call);
        }
    }

    template<typename Tcall>
    void for_each_reachable_node(const Tcall& call) {
        int_for_each_reachable_node(call);

        untag_nested_children();
    }

    template<typename Tcall>
    void for_each_reachable_edge(const Tcall& call) {
        int_for_each_reachable_edge(call);

        untag_nested_children();
    }


    template<typename Tcall>
    void make_edge(const Tcall& call) {
        m_children.push_back(std::make_pair(Tedge(), std::make_shared<graph_node<Tnode, Tedge>>()));
        edge(m_children.size() - 1, call);        
    }

    template<typename Tcall>
    void make_edge(const std::shared_ptr<graph_node<Tnode, Tedge>>& dest, const Tcall& call) {
        if (dest != nullptr) {
            m_children.push_back(std::make_pair(Tedge(), dest));
            edge(m_children.size() - 1, call);        
        }
    }

    void make_edge(const std::shared_ptr<graph_node<Tnode, Tedge>>& dest) {
        if (dest != nullptr) {
            m_children.push_back(std::make_pair(Tedge(), dest));
        }
    }

    std::string to_string() {
        std::string result = "[" + local.to_string() + "]\n";
        result += int_to_string(0);
        untag_nested_children();

        return result;
    }

    Tnode local; 

    void untag_nested_children() {
        m_tag = false;
        for (const auto& [edge, node] : m_children) {
            if (node != nullptr) {
                if (node->m_tag) {
                    node->untag_nested_children();
                }
            }
        }
    }
    bool& tag() {
        return m_tag;
    }
private:
    template<typename Tcall>
    void int_for_each_reachable_node(const Tcall& call) {
        m_tag = true;
        for (auto& [edge, node] : m_children) {
            if (node != nullptr) {
                call(node);
                if (!node->m_tag) {
                    node->int_for_each_reachable_node(call);
                }
            }
        }
    }

    template<typename Tcall>
    void int_for_each_reachable_edge(const Tcall& call) {
        m_tag = true;
        for (auto& [edge, node] : m_children) {
            if (node != nullptr) {
                call(*this, edge, node);
                if (!node->m_tag) {
                    node->int_for_each_reachable_edge(call);
                }
            }
        }
    }

    std::string int_to_string(const int level) {
        std::string result;

        m_tag = true;
        for (const auto& [edge, node] : m_children) {
            if (node != nullptr) {
                result += std::string(level, ' ') + " --[" + edge.to_string() + "]--> " + "[" + node->local.to_string() + "]\n";
                if (!node->m_tag) {
                    result += node->int_to_string(level + 4);
                }
            }
        }

        return result;
    }


    std::vector<std::pair<Tedge, std::shared_ptr<graph_node<Tnode, Tedge>>>> m_children;
    bool m_tag = false;
};
