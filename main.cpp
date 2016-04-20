#include <iostream>
#include "graph.hpp"

using namespace std;

int main() {
    Graph *graph = new Graph();
    graph->populate();
    graph->execute();
    delete graph;
    return 0;
}