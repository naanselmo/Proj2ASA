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
Vertex<T>::Vertex(T element) {
    this->element = element;
    this->reset();
}

template<class T>
void Vertex<T>::addLink(Vertex<T> *const vertex, int cost) {
    this->neighbours.push_back(new Edge<T>(vertex, cost));
}

template<class T>
inline void Vertex<T>::reset() {
    this->distance = INFINITE;
    this->parent = NULL;
}

template<class T>
void Vertex<T>::saveDistance() {
    this->h = this->distance;
}

template<class T>
Vertex<T>::~Vertex() {
    delete this->element;
    for (typename std::list<Edge<T> *>::iterator it = this->neighbours.begin(); it != this->neighbours.end(); it++) {
        Edge<T> *edge = *it;
        delete edge;
    }
}

//
// Graph (Class)
//

Graph::Graph() {
    this->placesLength = 0;
    this->branchesLength = 0;
}

void Graph::populate() {
    // Parse first line
    unsigned int linksLength;
    std::cin >> this->placesLength >> this->branchesLength >> linksLength;
    // places starts at 1 and 0 is saved for the s vertex in johnson's algorithm, so add 1 extra place for s.
    this->placesLength++;
    this->places = new Vertex<Place *> *[placesLength];
    this->places[S_INDEX] = NULL;
    this->branches = new Vertex<Place *> *[branchesLength];
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Place *place = new Place(placeIndex);
        this->places[placeIndex] = new Vertex<Place *>(place);
    }

    // Parse second line
    for (unsigned int branchIndex = 0; branchIndex < this->branchesLength; branchIndex++) {
        unsigned int id;
        std::cin >> id;
        Vertex<Place *> *vertex = this->places[id];
        Place *place = vertex->element;
        place->branch = new Branch();
        this->branches[branchIndex] = vertex;
    }

    // Parse connections
    for (unsigned int connection = 0; connection < linksLength; connection++) {
        unsigned int from, to;
        int cost;
        std::cin >> from >> to >> cost;

        // IDs start at 1, but we store them from 0
        this->places[from]->addLink(this->places[to], cost);
    }
}

void Graph::execute() {
    johnson();
}

void Graph::print() const {
    std::cout << "Places: " << this->placesLength - 1 << ", Branches: " << this->branchesLength << std::endl;
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = this->places[placeIndex];
        std::cout << "Place " << vertex->element->id << "[" << vertex << "]" << " has branch? " << (vertex->element->branch != NULL) << " is linked to: " << std::endl;
        std::list<Edge<Place *> *> links = vertex->neighbours;
        for (std::list<Edge<Place *> *>::iterator it = links.begin(); it != links.end(); it++) {
            Edge<Place *> *edge = *it;
            Vertex<Place *> *linked = edge->vertex;
            std::cout << "\t-> Place " << linked->element->id << "[" << linked << "] with cost " << edge->cost << " and has branch? " << (linked->element->branch != NULL) << std::endl;
        }
    }
}

bool Graph::bellmanFord(Vertex<Place *> *source) {
    // Initialize the graph
    for (unsigned int placeIndex = 0; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = places[placeIndex];
        if (vertex != NULL)
            vertex->reset();
    }
    source->distance = 0;

    // Relax edges
    for (unsigned int iteration = 0; iteration < this->placesLength; iteration++) {
        // For each edge, relax if possible
        for (unsigned int placeIndex = 0; placeIndex < this->placesLength; placeIndex++) {
            Vertex<Place *> *origin = places[placeIndex];
            if (origin != NULL) {
                std::list<Edge<Place *> *> neighbours = origin->neighbours;
                for (std::list<Edge<Place *> *>::iterator it = neighbours.begin(); it != neighbours.end(); it++) {
                    Edge<Place *> *edge = *it;
                    Vertex<Place *> *destination = edge->vertex;
                    // If can relax, do it! Check if origin is infinite so stuff doest overflow!
                    // Also sum overflow happens we do nothing to prevent it. SO DON'T MAKE EDGES HEIGHT HIGH!
                    if (origin->distance != INFINITE && origin->distance + edge->cost < destination->distance) {
                        destination->distance = origin->distance + edge->cost;
                        destination->parent = origin;
                    }
                }
            }
        }
    }

    // Check for negative cycles
    // Commented because there will be no negative loops, so no need for checks.
    /*for (unsigned int i = 0; i < this->placesLength; i++) {
        // For each edge, relax if possible
        for (unsigned int place = 0; place < this->placesLength; place++) {
            Vertex<Place *> *origin = places[place];
            std::list<Edge<Place *> *> neighbours = origin->neighbours;
            for (std::list<Edge<Place *> *>::iterator it = neighbours.begin(); it != neighbours.end(); it++) {
                Edge<Place *> *edge = *it;
                Vertex<Place *> *destination = edge->vertex;
                // If we can relax, its because there is a negative cycle
                if (origin->distance + edge->cost < destination->distance) {
                    return false;
                }
            }
        }
    }*/
    return true;
}

std::list<Vertex<Place *> *> Graph::path(Vertex<Place *> *destination) {
    std::list<Vertex<Place *> *> path;
    Vertex<Place *> *current = destination;
    while (current->parent != NULL) {
        path.push_front(current);
        current = current->parent;
    }
    path.push_front(current);
    return path;
}

void Graph::johnson() {
    // Create s vertex.
    this->places[S_INDEX] = new Vertex<Place *>(NULL);
    // Add a edge from s to every vertex with cost 0.
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        this->places[S_INDEX]->addLink(places[placeIndex], 0);
    }
    // Run bellmanFord in s.
    bellmanFord(this->places[S_INDEX]);

    // Save the distances calculated in vertex's h field
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = places[placeIndex];
        vertex->saveDistance();
    }

    // Re-weight the edges.
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *origin = places[placeIndex];
        std::list<Edge<Place *> *> neighbours = origin->neighbours;
        for (std::list<Edge<Place *> *>::iterator it = neighbours.begin(); it != neighbours.end(); it++) {
            Edge<Place *> *edge = *it;
            Vertex<Place *> *destination = edge->vertex;
            edge->cost = edge->cost + origin->h - destination->h;
        }
    }

    // Delete s to run Dijkstra.
    delete this->places[S_INDEX];
    this->places[S_INDEX] = NULL;

    // Run Dijkstra for all vertexes and update the distance matrix with the real distances by undoing the re-weight.
    // Also calculate total loss to all places.
    int distance[branchesLength][placesLength] = {0};
    int totalLoss[placesLength] = {0};
    for (unsigned int branchIndex = 0; branchIndex < branchesLength; branchIndex++) {
        Vertex<Place *> *source = branches[branchIndex];
        bellmanFord(source); // <- INSERT DIJKSTRA HERE
        for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < placesLength; placeIndex++) {
            Vertex<Place *> *destination = places[placeIndex];
            // If distance is infinite then will remain infinite since we can't reach the destination from source.
            if (destination->distance == INFINITE) {
                distance[branchIndex][placeIndex] = INFINITE;
                totalLoss[placeIndex] = INFINITE;
            } else {
                distance[branchIndex][placeIndex] = destination->distance + destination->h - source->h;
                if (totalLoss[placeIndex] != INFINITE) {
                    totalLoss[placeIndex] += distance[branchIndex][placeIndex];
                }
            }
        }
    }

    // Find the encounter place based on total loss.
    int minimumTotalLoss = INFINITE;
    int encounterPlaceIndex = -1;
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < placesLength; placeIndex++) {
        if (totalLoss[placeIndex] < minimumTotalLoss) {
            minimumTotalLoss = totalLoss[placeIndex];
            encounterPlaceIndex = placeIndex;
        }
    }

    // Print the output based on the encounter point.
    if (encounterPlaceIndex == -1) {
        std::cout << "N" << std::endl;
    } else {
        // std::cout << "Found encounter point: " << places[encounterPlaceIndex]->element->id << " with total loss " << minimumTotalLoss << std::endl;
        std::cout << places[encounterPlaceIndex]->element->id << " " << minimumTotalLoss << std::endl;
        for (unsigned int branchIndex = 0; branchIndex < branchesLength; branchIndex++) {
            std::cout << distance[branchIndex][encounterPlaceIndex] << " ";
            // std::cout << "Total loss from " << source->element->id << " to encounter point " << distance[branchIndex][encounterPlaceIndex] << std::endl;
        }
        std::cout << std::endl;
    }
}

Graph::~Graph() {
    delete[] this->branches;
    for (unsigned int i = 0; i < this->placesLength; ++i) {
        delete this->places[i];
    }
    delete[] this->places;
}