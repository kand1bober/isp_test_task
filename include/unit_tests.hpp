#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <optional>

#include "pool.hpp"

namespace term_color {
    constexpr const char* reset  = "\033[0m";
    constexpr const char* red    = "\033[31m";
    constexpr const char* green  = "\033[32m";
    constexpr const char* yellow = "\033[33m";
    constexpr const char* blue   = "\033[34m";
    constexpr const char* cyan   = "\033[36m";
    constexpr const char* bold   = "\033[1m";
}

class node_dispatcher_unit_tests {
private:
    struct assertion_result_t {
        bool passed;
        std::string label;
        int32_t expected;
        int32_t actual;
    };

    int passed_tests_ = 0;
    int failed_tests_ = 0;
    int passed_assertions_ = 0;
    int failed_assertions_ = 0;

    std::vector<assertion_result_t> current_test_assertions_;

    void begin_test(const std::string& test_name) {
        current_test_assertions_.clear();

        std::cout << "\n============================================================\n";
        std::cout << term_color::bold << term_color::blue
                  << "TEST: " << test_name
                  << term_color::reset << "\n";
        std::cout << "------------------------------------------------------------\n";
    }

    void check_equal(int32_t actual, int32_t expected, const std::string& label) {
        const bool ok = (actual == expected);
        current_test_assertions_.push_back({ok, label, expected, actual});

        if (ok) {
            ++passed_assertions_;
        } else {
            ++failed_assertions_;
        }
    }

    void check_true(bool actual, const std::string& label) {
        const bool ok = actual;
        current_test_assertions_.push_back({ok, label, 1, actual ? 1 : 0});

        if (ok) {
            ++passed_assertions_;
        } else {
            ++failed_assertions_;
        }
    }

    void check_false(bool actual, const std::string& label) {
        const bool ok = !actual;
        current_test_assertions_.push_back({ok, label, 0, actual ? 1 : 0});

        if (ok) {
            ++passed_assertions_;
        } else {
            ++failed_assertions_;
        }
    }

    void check_optional_equal(const std::optional<int32_t>& actual,
                              int32_t expected,
                              const std::string& label) {
        const bool ok = actual.has_value() && (*actual == expected);
        current_test_assertions_.push_back({
            ok,
            label,
            expected,
            actual.has_value() ? *actual : -999999999
        });

        if (ok) {
            ++passed_assertions_;
        } else {
            ++failed_assertions_;
        }
    }

    void check_optional_empty(const std::optional<int32_t>& actual,
                              const std::string& label) {
        const bool ok = !actual.has_value();
        current_test_assertions_.push_back({
            ok,
            label,
            0,
            actual.has_value() ? *actual : 0
        });

        if (ok) {
            ++passed_assertions_;
        } else {
            ++failed_assertions_;
        }
    }

    int count_passed_current_test() const {
        int count = 0;
        for (const auto& a : current_test_assertions_) {
            if (a.passed) {
                ++count;
            }
        }
        return count;
    }

    int count_failed_current_test() const {
        int count = 0;
        for (const auto& a : current_test_assertions_) {
            if (!a.passed) {
                ++count;
            }
        }
        return count;
    }

    void end_test() {
        bool test_passed = true;

        for (const auto& a : current_test_assertions_) {
            if (!a.passed) {
                test_passed = false;
                break;
            }
        }

        for (const auto& a : current_test_assertions_) {
            std::cout
                << (a.passed
                    ? std::string(term_color::green) + "[ OK ] " + term_color::reset
                    : std::string(term_color::red)   + "[FAIL] " + term_color::reset)
                << a.label
                << " | expected = " << a.expected
                << ", actual = " << a.actual
                << "\n";
        }

        std::cout << "------------------------------------------------------------\n";
        std::cout << term_color::bold << "RESULT: " << term_color::reset
                  << (test_passed
                        ? std::string(term_color::green) + "PASSED"
                        : std::string(term_color::red)   + "FAILED")
                  << term_color::reset
                  << " | assertions: " << current_test_assertions_.size()
                  << ", passed: " << count_passed_current_test()
                  << ", failed: " << count_failed_current_test()
                  << "\n";

        if (test_passed) {
            ++passed_tests_;
        } else {
            ++failed_tests_;
        }
    }

    void print_node_values(node_dispatcher& graph, int32_t from, int32_t to, const std::string& title) {
        std::cout << term_color::cyan << title << term_color::reset << ": ";
        for (int32_t i = from; i <= to; ++i) {
            const auto value = graph.show_amount_of_resource(i);
            std::cout << "[" << i << "]=";
            if (value.has_value()) {
                std::cout << *value;
            } else {
                std::cout << "null";
            }

            if (i != to) {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }

    void add_nodes(node_dispatcher& graph, int count) {
        for (int i = 0; i < count; ++i) {
            graph.add_node();
        }
    }

    void test_single_node_resource() {
        begin_test("single node keeps its own resource");

        node_dispatcher graph;
        add_nodes(graph, 1);
        check_true(graph.add_resource(0, 100), "add_resource(0, 100) succeeds");

        std::cout << term_color::cyan
                  << "Scenario: one isolated node with resource 100"
                  << term_color::reset << "\n";
        print_node_values(graph, 0, 0, "Before");

        graph.calculate_resources();

        print_node_values(graph, 0, 0, "After");
        check_optional_equal(graph.show_amount_of_resource(0), 100, "node 0 keeps 100");

        end_test();
    }

    void test_two_isolated_nodes() {
        begin_test("two isolated nodes keep independent values");

        node_dispatcher graph;
        add_nodes(graph, 2);
        check_true(graph.add_resource(0, 11), "add_resource(0, 11) succeeds");
        check_true(graph.add_resource(1, 29), "add_resource(1, 29) succeeds");

        print_node_values(graph, 0, 1, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 1, "After");

        check_optional_equal(graph.show_amount_of_resource(0), 11, "node 0 stays 11");
        check_optional_equal(graph.show_amount_of_resource(1), 29, "node 1 stays 29");

        end_test();
    }

    void test_two_connected_nodes_average() {
        begin_test("two connected nodes are averaged");

        node_dispatcher graph;
        add_nodes(graph, 2);
        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 30), "add_resource(1, 30) succeeds");
        graph.add_edge(0, 1);

        std::cout << term_color::cyan
                  << "Scenario: 0--1 with resources 10 and 30"
                  << term_color::reset << "\n";
        print_node_values(graph, 0, 1, "Before");

        graph.calculate_resources();

        print_node_values(graph, 0, 1, "After");
        check_optional_equal(graph.show_amount_of_resource(0), 20, "node 0 becomes 20");
        check_optional_equal(graph.show_amount_of_resource(1), 20, "node 1 becomes 20");

        end_test();
    }

    void test_three_node_chain() {
        begin_test("chain of three nodes is averaged");

        node_dispatcher graph;
        add_nodes(graph, 3);
        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 20), "add_resource(1, 20) succeeds");
        check_true(graph.add_resource(2, 30), "add_resource(2, 30) succeeds");
        graph.add_edge(0, 1);
        graph.add_edge(1, 2);

        print_node_values(graph, 0, 2, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 2, "After");

        check_optional_equal(graph.show_amount_of_resource(0), 20, "node 0 becomes 20");
        check_optional_equal(graph.show_amount_of_resource(1), 20, "node 1 becomes 20");
        check_optional_equal(graph.show_amount_of_resource(2), 20, "node 2 becomes 20");

        end_test();
    }

    void test_four_node_cycle() {
        begin_test("cycle of four nodes is processed as one component");

        node_dispatcher graph;
        add_nodes(graph, 4);
        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 20), "add_resource(1, 20) succeeds");
        check_true(graph.add_resource(2, 30), "add_resource(2, 30) succeeds");
        check_true(graph.add_resource(3, 40), "add_resource(3, 40) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 3);
        graph.add_edge(3, 0);

        print_node_values(graph, 0, 3, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 3, "After");

        check_optional_equal(graph.show_amount_of_resource(0), 25, "node 0 becomes 25");
        check_optional_equal(graph.show_amount_of_resource(1), 25, "node 1 becomes 25");
        check_optional_equal(graph.show_amount_of_resource(2), 25, "node 2 becomes 25");
        check_optional_equal(graph.show_amount_of_resource(3), 25, "node 3 becomes 25");

        end_test();
    }

    void test_two_components() {
        begin_test("two connected components are averaged independently");

        node_dispatcher graph;
        add_nodes(graph, 5);

        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 30), "add_resource(1, 30) succeeds");
        check_true(graph.add_resource(2, 100), "add_resource(2, 100) succeeds");
        check_true(graph.add_resource(3, 200), "add_resource(3, 200) succeeds");
        check_true(graph.add_resource(4, 50), "add_resource(4, 50) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(2, 3);

        print_node_values(graph, 0, 4, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 4, "After");

        check_optional_equal(graph.show_amount_of_resource(0), 20,  "component {0,1}: node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 20,  "component {0,1}: node 1");
        check_optional_equal(graph.show_amount_of_resource(2), 150, "component {2,3}: node 2");
        check_optional_equal(graph.show_amount_of_resource(3), 150, "component {2,3}: node 3");
        check_optional_equal(graph.show_amount_of_resource(4), 50,  "isolated node 4");

        end_test();
    }

    void test_remove_edge_from_chain() {
        begin_test("removing edge splits a three-node chain");

        node_dispatcher graph;
        add_nodes(graph, 3);

        check_true(graph.add_resource(0, 0), "add_resource(0, 0) succeeds");
        check_true(graph.add_resource(1, 30), "add_resource(1, 30) succeeds");
        check_true(graph.add_resource(2, 60), "add_resource(2, 60) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);

        print_node_values(graph, 0, 2, "Before first calculate");
        graph.calculate_resources();
        print_node_values(graph, 0, 2, "After first calculate");

        check_optional_equal(graph.show_amount_of_resource(0), 30, "before remove: node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 30, "before remove: node 1");
        check_optional_equal(graph.show_amount_of_resource(2), 30, "before remove: node 2");

        graph.remove_edge(1, 2);
        std::cout << term_color::cyan << "Action: remove edge (1,2)" << term_color::reset << "\n";

        graph.calculate_resources();
        print_node_values(graph, 0, 2, "After second calculate");

        check_optional_equal(graph.show_amount_of_resource(0), 30, "after remove: node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 30, "after remove: node 1");
        check_optional_equal(graph.show_amount_of_resource(2), 30, "after remove: node 2 isolated");

        end_test();
    }

    void test_add_resource_after_balance() {
        begin_test("resource can be added after previous balancing");

        node_dispatcher graph;
        add_nodes(graph, 2);

        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 30), "add_resource(1, 30) succeeds");
        graph.add_edge(0, 1);

        graph.calculate_resources();
        print_node_values(graph, 0, 1, "After first calculate");

        check_optional_equal(graph.show_amount_of_resource(0), 20, "first balance node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 20, "first balance node 1");

        check_true(graph.add_resource(0, 20), "add_resource(0, 20) succeeds");
        std::cout << term_color::cyan << "Action: add 20 to node 0" << term_color::reset << "\n";

        graph.calculate_resources();
        print_node_values(graph, 0, 1, "After second calculate");

        check_optional_equal(graph.show_amount_of_resource(0), 30, "second balance node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 30, "second balance node 1");

        end_test();
    }

    void test_integer_division_round_down() {
        begin_test("average uses integer division");

        node_dispatcher graph;
        add_nodes(graph, 2);

        check_true(graph.add_resource(0, 1), "add_resource(0, 1) succeeds");
        check_true(graph.add_resource(1, 2), "add_resource(1, 2) succeeds");
        graph.add_edge(0, 1);

        print_node_values(graph, 0, 1, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 1, "After");

        check_optional_equal(graph.show_amount_of_resource(0), 1, "node 0 gets floor(3/2)=1");
        check_optional_equal(graph.show_amount_of_resource(1), 1, "node 1 gets floor(3/2)=1");

        end_test();
    }

    void test_star_of_six_nodes() {
        begin_test("star graph of six nodes is averaged");

        node_dispatcher graph;
        add_nodes(graph, 6);

        check_true(graph.add_resource(0, 60), "add_resource(0, 60) succeeds");
        check_true(graph.add_resource(1, 0), "add_resource(1, 0) succeeds");
        check_true(graph.add_resource(2, 0), "add_resource(2, 0) succeeds");
        check_true(graph.add_resource(3, 0), "add_resource(3, 0) succeeds");
        check_true(graph.add_resource(4, 0), "add_resource(4, 0) succeeds");
        check_true(graph.add_resource(5, 0), "add_resource(5, 0) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(0, 3);
        graph.add_edge(0, 4);
        graph.add_edge(0, 5);

        std::cout << term_color::cyan
                  << "Scenario: 6 nodes, star centered at 0"
                  << term_color::reset << "\n";
        print_node_values(graph, 0, 5, "Before");

        graph.calculate_resources();

        print_node_values(graph, 0, 5, "After");

        for (int i = 0; i < 6; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 10, "star graph node " + std::to_string(i));
        }

        end_test();
    }

    void test_seven_node_two_components() {
        begin_test("seven nodes split into two nontrivial components");

        node_dispatcher graph;
        add_nodes(graph, 7);

        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 20), "add_resource(1, 20) succeeds");
        check_true(graph.add_resource(2, 30), "add_resource(2, 30) succeeds");
        check_true(graph.add_resource(3, 40), "add_resource(3, 40) succeeds");
        check_true(graph.add_resource(4, 50), "add_resource(4, 50) succeeds");
        check_true(graph.add_resource(5, 60), "add_resource(5, 60) succeeds");
        check_true(graph.add_resource(6, 70), "add_resource(6, 70) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(3, 4);
        graph.add_edge(4, 5);

        print_node_values(graph, 0, 6, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 6, "After");

        check_optional_equal(graph.show_amount_of_resource(0), 20, "component {0,1,2}: node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 20, "component {0,1,2}: node 1");
        check_optional_equal(graph.show_amount_of_resource(2), 20, "component {0,1,2}: node 2");
        check_optional_equal(graph.show_amount_of_resource(3), 50, "component {3,4,5}: node 3");
        check_optional_equal(graph.show_amount_of_resource(4), 50, "component {3,4,5}: node 4");
        check_optional_equal(graph.show_amount_of_resource(5), 50, "component {3,4,5}: node 5");
        check_optional_equal(graph.show_amount_of_resource(6), 70, "isolated node 6");

        end_test();
    }

    void test_six_node_chain_with_middle_split() {
        begin_test("six-node chain split in the middle");

        node_dispatcher graph;
        add_nodes(graph, 6);

        check_true(graph.add_resource(0, 0), "add_resource(0, 0) succeeds");
        check_true(graph.add_resource(1, 0), "add_resource(1, 0) succeeds");
        check_true(graph.add_resource(2, 0), "add_resource(2, 0) succeeds");
        check_true(graph.add_resource(3, 60), "add_resource(3, 60) succeeds");
        check_true(graph.add_resource(4, 60), "add_resource(4, 60) succeeds");
        check_true(graph.add_resource(5, 60), "add_resource(5, 60) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 3);
        graph.add_edge(3, 4);
        graph.add_edge(4, 5);

        graph.calculate_resources();
        print_node_values(graph, 0, 5, "After first calculate");

        for (int i = 0; i < 6; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 30, "before split node " + std::to_string(i));
        }

        graph.remove_edge(2, 3);
        std::cout << term_color::cyan << "Action: remove edge (2,3)" << term_color::reset << "\n";

        graph.calculate_resources();
        print_node_values(graph, 0, 5, "After second calculate");

        check_optional_equal(graph.show_amount_of_resource(0), 30, "left part node 0");
        check_optional_equal(graph.show_amount_of_resource(1), 30, "left part node 1");
        check_optional_equal(graph.show_amount_of_resource(2), 30, "left part node 2");
        check_optional_equal(graph.show_amount_of_resource(3), 30, "right part node 3");
        check_optional_equal(graph.show_amount_of_resource(4), 30, "right part node 4");
        check_optional_equal(graph.show_amount_of_resource(5), 30, "right part node 5");

        end_test();
    }

    void test_seven_node_cycle_plus_tail() {
        begin_test("seven-node graph: cycle plus tail still one component");

        node_dispatcher graph;
        add_nodes(graph, 7);

        for (int i = 0; i < 7; ++i) {
            check_true(graph.add_resource(i, (i + 1) * 10),
                       "add_resource(" + std::to_string(i) + ", " + std::to_string((i + 1) * 10) + ") succeeds");
        }

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 3);
        graph.add_edge(3, 0);
        graph.add_edge(3, 4);
        graph.add_edge(4, 5);
        graph.add_edge(5, 6);

        print_node_values(graph, 0, 6, "Before");
        graph.calculate_resources();
        print_node_values(graph, 0, 6, "After");

        for (int i = 0; i < 7; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 40, "cycle+tail node " + std::to_string(i));
        }

        end_test();
    }

    void test_remove_non_bridge_edge_keeps_component_connected() {
        begin_test("removing non-bridge edge from cycle keeps one component");

        node_dispatcher graph;
        add_nodes(graph, 4);

        check_true(graph.add_resource(0, 10), "add_resource(0, 10) succeeds");
        check_true(graph.add_resource(1, 20), "add_resource(1, 20) succeeds");
        check_true(graph.add_resource(2, 30), "add_resource(2, 30) succeeds");
        check_true(graph.add_resource(3, 40), "add_resource(3, 40) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 3);
        graph.add_edge(3, 0);

        graph.calculate_resources();
        print_node_values(graph, 0, 3, "After first calculate");

        for (int i = 0; i < 4; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 25, "before edge removal node " + std::to_string(i));
        }

        graph.remove_edge(0, 1);
        std::cout << term_color::cyan << "Action: remove edge (0,1) from cycle" << term_color::reset << "\n";

        graph.calculate_resources();
        print_node_values(graph, 0, 3, "After second calculate");

        for (int i = 0; i < 4; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 25, "after non-bridge removal node " + std::to_string(i));
        }

        end_test();
    }

    void test_repeated_balance_without_changes() {
        begin_test("repeated balancing without graph changes is stable");

        node_dispatcher graph;
        add_nodes(graph, 4);

        check_true(graph.add_resource(0, 8), "add_resource(0, 8) succeeds");
        check_true(graph.add_resource(1, 16), "add_resource(1, 16) succeeds");
        check_true(graph.add_resource(2, 24), "add_resource(2, 24) succeeds");
        check_true(graph.add_resource(3, 32), "add_resource(3, 32) succeeds");

        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 3);

        graph.calculate_resources();
        print_node_values(graph, 0, 3, "After first calculate");

        for (int i = 0; i < 4; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 20, "first pass node " + std::to_string(i));
        }

        graph.calculate_resources();
        print_node_values(graph, 0, 3, "After second calculate");

        for (int i = 0; i < 4; ++i) {
            check_optional_equal(graph.show_amount_of_resource(i), 20, "second pass node " + std::to_string(i));
        }

        end_test();
    }

    void test_invalid_negative_resource_is_rejected() {
        begin_test("adding negative resource is rejected");

        node_dispatcher graph;
        add_nodes(graph, 1);

        check_true(graph.add_resource(0, 50), "add_resource(0, 50) succeeds");
        const bool rc = graph.add_resource(0, -10);

        print_node_values(graph, 0, 0, "Before calculate");
        graph.calculate_resources();
        print_node_values(graph, 0, 0, "After calculate");

        check_false(rc, "add_resource returns false on negative input");
        check_optional_equal(graph.show_amount_of_resource(0), 50, "resource remains unchanged");

        end_test();
    }

public:
    bool run_all() {
        std::cout << term_color::bold << term_color::yellow
                  << "RUNNING UNIT TESTS FOR node_dispatcher"
                  << term_color::reset << "\n";

        test_single_node_resource();
        test_two_isolated_nodes();
        test_two_connected_nodes_average();
        test_three_node_chain();
        test_four_node_cycle();
        test_two_components();
        test_remove_edge_from_chain();
        test_add_resource_after_balance();
        test_integer_division_round_down();
        test_star_of_six_nodes();
        test_seven_node_two_components();
        test_six_node_chain_with_middle_split();
        test_seven_node_cycle_plus_tail();
        test_remove_non_bridge_edge_keeps_component_connected();
        test_repeated_balance_without_changes();
        test_invalid_negative_resource_is_rejected();

        std::cout << "\n============================================================\n";
        std::cout << term_color::bold << term_color::yellow
                  << "FINAL SUMMARY"
                  << term_color::reset << "\n";
        std::cout << "------------------------------------------------------------\n";

        std::cout << "Tests passed   : "
                  << term_color::green << passed_tests_ << term_color::reset << "\n";

        std::cout << "Tests failed   : "
                  << (failed_tests_ == 0 ? term_color::green : term_color::red)
                  << failed_tests_ << term_color::reset << "\n";

        std::cout << "Assertions pass: "
                  << term_color::green << passed_assertions_ << term_color::reset << "\n";

        std::cout << "Assertions fail: "
                  << (failed_assertions_ == 0 ? term_color::green : term_color::red)
                  << failed_assertions_ << term_color::reset << "\n";

        return failed_tests_ == 0;
    }
};