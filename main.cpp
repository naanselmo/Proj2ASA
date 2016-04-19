#include <iostream>
#include "graph.hpp"

using namespace std;

int main() {
    Graph *graph = new Graph();
    graph->populate();
    graph->execute();
    graph->print();
    delete graph;
    return 0;
}