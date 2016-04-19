#include <iostream>
#include "graph.hpp"

//
// Edge (Template)
//

template<class T>
Edge<T>::Edge(Vertex<T> *const vertex, int cost) {
    this->vertex = vertex;
    this->cost = cost;
}

template<class T>
Edge<T>::~Edge() {

}

//
// Vertex (Template)
//

template<class T>
Vertex<T>::Vertex(T element)
{
    this->element = element;
    this->reset();
}

template<class T>
void Vertex<T>::addLink(Vertex<T>* const vertex, int cost)
{
    this->neighbours.push_back(new Edge<T>(vertex, cost));
}

template<class T>
inline void Vertex<T>::reset()
{
    this->distance = INFINITE;
    this->parent = NULL;
}

template<class T>
Vertex<T>::~Vertex()
{
    delete this->element;
    for (typename std::list<Edge<T> *>::iterator it = this->neighbours.begin(); it != this->neighbours.end(); it++){
        Edge<T> *edge = *it;
        delete edge;
    }
}

//
// Graph (Class)
//

Graph::Graph()
{
    this->placesLength = 0;
    this->branchesLength = 0;
}

void Graph::populate()
{
    // Parse first line
    unsigned int linksLength;
    std::cin >> this->placesLength >> this->branchesLength >> linksLength;
    this->places = new Vertex<Place *> *[placesLength];
    this->branches = new Vertex<Place *> *[branchesLength];
    for (unsigned int id = 0; id < this->placesLength; id++) {
        Place *place = new Place(id + 1);
        this->places[id] = new Vertex<Place *>(place);
    }

    // Parse second line
    for (unsigned int branch = 0; branch < this->branchesLength; branch++){
        unsigned int id;
        std::cin >> id;
        Vertex<Place *> *vertex = this->places[--id];
        Place *place = vertex->element;
        place->branch = new Branch();
        this->branches[branch] = vertex;
    }

    // Parse connections
    for (unsigned int connection = 0; connection < linksLength; connection++) {
        unsigned int from, to;
        int cost;
        std::cin >> from >> to >> cost;

        // IDs start at 1, but we store them from 0
        this->places[--from]->addLink(this->places[--to], cost);
    }
}

void Graph::execute()
{
}

void Graph::print() const
{
    std::cout << "Places: " << this->placesLength << ", Branches: " << this->branchesLength << std::endl;
    for (unsigned int i = 0; i < this->placesLength; i++) {
        Vertex<Place *> *vertex = this->places[i];
        std::cout << "Place " << vertex->element->id << "[" << vertex << "]" << " has branch? " << (vertex->element->branch != NULL) << " is linked to: " << std::endl;
        std::list<Edge<Place *> *> links = vertex->neighbours;
        for (std::list<Edge<Place *> *>::iterator it = links.begin(); it != links.end(); it++) {
            Edge<Place *> *edge = *it;
            Vertex<Place *> *linked = edge->vertex;
            std::cout << "\t-> Place " << linked->element->id << "[" << linked << "] with cost " << edge->cost << " and has branch? " << (linked->element->branch != NULL) << std::endl;
        }
    }
}

Graph::~Graph()
{
    delete[] this->branches;
    for (unsigned int i = 0; i < this->placesLength; ++i) {
        delete this->places[i];
    }
    delete[] this->places;
}