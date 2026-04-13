#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "../include/pool.hpp"

using namespace std;
using namespace std::chrono;

// быстрый RNG
static std::mt19937 rng(123456);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int32_t N = 38'000'000;
    const int32_t K = 13'000'000;
    const int32_t L = 31'000'000;
    const int32_t M = 4'000'000;

    node_dispatcher graph;

    // ---------- 1. create nodes ----------
    auto t0 = high_resolution_clock::now();

    graph = node_dispatcher();
    for (int32_t i = 0; i < N; ++i) {
        graph.add_node();
    }

    auto t1 = high_resolution_clock::now();
    cout << "Create nodes: " << duration_cast<seconds>(t1 - t0).count() << " sec\n";

    // ---------- distributions ----------
    uniform_int_distribution<int32_t> node_dist(0, N - 1);
    uniform_int_distribution<int32_t> water_dist(1, 500);

    // ---------- 2. initial water ----------
    for (int32_t i = 0; i < N; ++i) {
        graph.add_resource(i, water_dist(rng));
    }

    auto t2 = high_resolution_clock::now();
    cout << "Initial fill: " << duration_cast<seconds>(t2 - t1).count() << " sec\n";

    // ---------- 3. connect K edges ----------
    vector<pair<int32_t,int32_t>> edges;
    edges.reserve(K);

    for (int32_t i = 0; i < K; ++i) {
        int32_t a = node_dist(rng);
        int32_t b = node_dist(rng);
        if (a != b) {
            graph.add_connection(a, b);
            edges.emplace_back(a, b);
        }
    }

    auto t3 = high_resolution_clock::now();
    cout << "Add connections: " << duration_cast<seconds>(t3 - t2).count() << " sec\n";

    // ---------- 4. measure ----------
    graph.calculate_resources();
    // <---here measuring of volumes of nodes can be placed 

    auto t4 = high_resolution_clock::now();
    cout << "First balancing: " << duration_cast<seconds>(t4 - t3).count() << " sec\n";

    // ---------- 5. add water L ----------
    for (int32_t i = 0; i < L; ++i) {
        int32_t node = node_dist(rng);
        graph.add_resource(node, water_dist(rng));
    }

    auto t5 = high_resolution_clock::now();
    cout << "Add water: " << duration_cast<seconds>(t5 - t4).count() << " sec\n";

    // ---------- 6. measure ----------
    graph.calculate_resources();
    // <---here measuring of volumes of nodes can be placed 

    auto t6 = high_resolution_clock::now();
    cout << "Second balancing: " << duration_cast<seconds>(t6 - t5).count() << " sec\n";

    // ---------- 7. remove M edges ----------
    for (size_t i = 0; i < M && i < edges.size(); ++i) {
        auto [a, b] = edges[i];
        graph.remove_edge(a, b);
    }

    auto t7 = high_resolution_clock::now();
    cout << "Remove edges: " << duration_cast<seconds>(t7 - t6).count() << " sec\n";

    // ---------- 8. add water again ----------
    for (int32_t i = 0; i < L; ++i) {
        int32_t node = node_dist(rng);
        graph.add_resource(node, water_dist(rng));
    }

    auto t8 = high_resolution_clock::now();
    cout << "Add water again: " << duration_cast<seconds>(t8 - t7).count() << " sec\n";

    // ---------- 9. measure ----------
    graph.calculate_resources();
    // <---here measuring of volumes of nodes can be placed 

    auto t9 = high_resolution_clock::now();
    cout << "Final balancing: " << duration_cast<seconds>(t9 - t8).count() << " sec\n";

    cout << "TOTAL: " << duration_cast<seconds>(t9 - t0).count() << " sec\n";

    return 0;
}
