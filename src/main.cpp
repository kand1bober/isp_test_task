#include "../include/pool.hpp"
#include <iostream>

void show_resource(dsu& graph) {
    for (int i = 0; i < 6; ++i) {
    std::cout << "Node " << i 
                << " volume: " 
                << graph.check_resource(i) 
                << "\n";
    }

    std::cout << "\n";
}

int main() {
    dsu graph;

    // создаём 6 узлов
    for (int i = 0; i < 6; ++i) {
        graph.make_set();
    }

    // создаём 2 компоненты:
    // 0-1-2
    graph.add_connection(0, 1);
    graph.add_connection(1, 2);

    // 3-4
    graph.add_connection(3, 4);

    // 5 — одиночная

    // задаём ресурсы
    graph.add_resource(0, 10);
    graph.add_resource(1, 20);
    graph.add_resource(2, 30);

    graph.add_resource(3, 100);
    graph.add_resource(4, 200);

    graph.add_resource(5, 50);

    show_resource(graph);
    // строим "MST" (по факту компоненты + балансировка)
    graph.make_minimum_spanning_tree();
    show_resource(graph);

    graph.add_connection(5, 4);
    graph.make_minimum_spanning_tree();
    show_resource(graph);

    graph.remove_connection(5, 4);
    graph.make_minimum_spanning_tree();
    show_resource(graph);

    graph.add_resource(3, 50);
    graph.make_minimum_spanning_tree();
    show_resource(graph);


    return 0;
}