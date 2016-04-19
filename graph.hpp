#ifndef GRAPH_H
#define GRAPH_H

#include <stack>
#include <map>
#include <list>
#include <vector>
#include <limits>
#include "branch.hpp"
#include "place.hpp"

#define INFINITE std::numeric_limits<int>::max()

template<class T>
class Vertex;

template<class T>
class Edge {
public:
    Vertex<T> *vertex;
    int cost;

    Edge(Vertex<T> *const vertex, int cost);           // Creates a new edge
    virtual ~Edge();                                   // Deconstructs a edge
};

template<class T>
class Vertex {
public:
    T element;
    Vertex *parent;
    int distance;
    std::list<Edge<T> *> neighbours;

    Vertex(T element);                                   // Creates a new vertex
    void reset();                                        // Resets a vertex to its initial state
    void addLink(Vertex<T> *const vertex, int cost);     // Connects the vertex with another one
    virtual ~Vertex();                                   // Deconstructs a vertex
};

class Graph {
    Vertex<Place *> **branches;
    unsigned int branchesLenght;
    Vertex<Place *> **places;
    unsigned int placesLength;

public:
    Graph();               // Creates a new graph
    void populate();       // Executes the algorithm
    void execute();        // Executes the algorithm
    void print() const;    // Prints the graph
    virtual ~Graph();      // Deconstructs a graph
};

#endif //GRAPH_H