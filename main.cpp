#include <iostream>
#include "graph.hpp"

int main() {
    Graph *graph = new Graph();
    graph->populate();
    graph->execute();
    delete graph;
    return 0;
}
