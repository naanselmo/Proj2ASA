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
#define S_INDEX 0
#define PLACES_START_INDEX 1

template<class T>
class Vertex;

template<class T>
class Edge {
public:
    Vertex<T> *vertex;
    int cost;

    Edge(Vertex<T> *const vertex, int cost);           // Creates a new edge.
    virtual ~Edge();                                   // Deconstructs a edge.
};

template<class T>
class Vertex {
public:
    T element;
    Vertex *parent;
    int distance;
    int h;
    std::list<Edge<T> *> neighbours;

    Vertex(T element);                                   // Creates a new vertex.
    void reset();                                        // Resets a vertex to its initial state.
    void addLink(Vertex<T> *const vertex, int cost);     // Connects the vertex with another one.
    void saveDistance();                                 // Saves distance field in h field.
    virtual ~Vertex();                                   // Deconstructs a vertex.
};

class Graph {
    Vertex<Place *> **branches;
    unsigned int branchesLength;
    Vertex<Place *> **places;
    unsigned int placesLength;

public:
    Graph();               // Creates a new graph.
    void populate();       // Populates the graph with the given input.
    void execute();        // Executes the algorithm.
    void print() const;    // Prints the graph.
    virtual ~Graph();      // Deconstructs a graph.

private:
    /**
     * Runs bellman-ford algorithm in the source and updates the graph distance and parent values of all vertexes.
     */
    bool bellmanFord(Vertex<Place *> *source);

    /**
     * Runs johnson's algorithm on the graph (only on branches) and prints the output of the project.
     */
    void johnson();

    /**
     * Reconstructs a path based on parent values of the graph, from a given destination.
     * Be sure to reconstruct only if the parent values are correct.
     */
    std::list<Vertex<Place *> *> path(Vertex<Place *> *destination);
};

#endif //GRAPH_H