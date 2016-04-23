#include <iostream>
#include <list>
#include <limits>
#include <queue>

#define INFINITE std::numeric_limits<int>::max()
#define S_INDEX 0
#define PLACES_START_INDEX 1

class Place {
public:
    unsigned int id;

    Place(unsigned int id);
    virtual ~Place();
};

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
    int distance;
    int h;
    std::list<Edge<T> > edges;

    Vertex(T element);                                   // Creates a new vertex.
    void reset();                                        // Resets a vertex to its initial state.
    void addLink(Vertex<T> *const vertex, int cost);     // Connects the vertex with another one.
    void saveDistance();                                 // Saves distance field in h field.
    std::list<Edge<T> > &getEdges();                     // Returns the edges
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
    // Runs Bellman-Ford algorithm in the source and updates the graph distance of all vertices.
    bool bellmanFord(Vertex<Place *> *source);

    // Runs Dijkstra's algorithm on the graph (only on branches).
    void dijkstra(Vertex<Place *> *source);

    // Transposes the graph.
    void transpose();
};

Place::Place(unsigned int id) {
    this->id = id;
}

Place::~Place() {
}

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
    this->edges.push_back(Edge<T>(vertex, cost));
}

template<class T>
inline void Vertex<T>::reset() {
    this->distance = INFINITE;
}

template<class T>
void Vertex<T>::saveDistance() {
    this->h = this->distance;
}

template<class T>
inline std::list<Edge<T> > &Vertex<T>::getEdges() {
    return this->edges;
}

template<class T>
Vertex<T>::~Vertex() {
    delete this->element;
}

struct distance_less {
    template<class T>
    bool operator()(const Vertex<T>* left, const Vertex<T>* right) const {
        return left->distance > right->distance;
    }
};

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

    // Save vertex 0 for Johnson's algorithm
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
        this->branches[branchIndex] = this->places[id];
    }

    // Parse connections
    for (unsigned int connection = 0; connection < linksLength; connection++) {
        unsigned int from, to;
        int cost;
        std::cin >> from >> to >> cost;
        this->places[from]->addLink(this->places[to], cost);
    }
}

void Graph::execute() {
    // Create the S vertex.
    this->places[S_INDEX] = new Vertex<Place *>(NULL);
    // Add a edge from S to every vertex with cost 0.
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        this->places[S_INDEX]->addLink(places[placeIndex], 0);
    }
    // Run the Bellman-Ford algorithm starting from S.
    bellmanFord(this->places[S_INDEX]);

    // Save the distances calculated in vertices' h field
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = places[placeIndex];
        vertex->saveDistance();
    }

    // Re-weight the edges.
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *origin = places[placeIndex];
        std::list<Edge<Place *> > &edges = origin->getEdges();
        for (std::list<Edge<Place *> >::iterator it = edges.begin(); it != edges.end(); it++) {
            Edge<Place *> &edge = *it;
            Vertex<Place *> *destination = edge.vertex;
            edge.cost = edge.cost + origin->h - destination->h;
        }
    }

    // Delete S as it's no longer needed.
    delete this->places[S_INDEX];
    this->places[S_INDEX] = NULL;

    // Array that will contain the total losses per place
    int totalLoss[placesLength];
    std::fill(totalLoss, totalLoss + placesLength, 0);

    // Run Dijkstra and calculate total loss to every place, from each branch.
    for (unsigned int branchIndex = 0; branchIndex < branchesLength; branchIndex++) {
        Vertex<Place *> *source = branches[branchIndex];
        dijkstra(source);
        for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < placesLength; placeIndex++) {
            Vertex<Place *> *destination = places[placeIndex];

            // If distance is infinite then distance will remain infinite since we can't reach the destination from source.
            // Only reason we do this rather than just sum infinite with infinite is to prevent integer overflows.
            if (destination->distance == INFINITE) {
                totalLoss[placeIndex] = INFINITE;
            } else if (totalLoss[placeIndex] != INFINITE) {
                totalLoss[placeIndex] += destination->distance + destination->h - source->h;
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
        Vertex<Place *> *encounterPlace = places[encounterPlaceIndex];

        // Rather than calculating each distance again, it's easier to transpose the graph.
        // After that's done, simply calculate the distance from the encounter point to each branch.
        // This is more efficient if two or more branches exist
        transpose();
        dijkstra(encounterPlace);

        std::cout << encounterPlace->element->id << " " << minimumTotalLoss << std::endl;
        for (unsigned int branchIndex = 0; branchIndex < branchesLength; branchIndex++) {
            Vertex<Place *> *branch = branches[branchIndex];
            std::cout << branch->distance + encounterPlace->h - branch->h << " ";
        }
        std::cout << std::endl;
    }
}

void Graph::print() const {
    std::cout << "Places: " << this->placesLength - 1 << ", Branches: " << this->branchesLength << std::endl;
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = this->places[placeIndex];
        std::cout << "Place " << vertex->element->id << "[" << vertex << "]" << " is linked to: " << std::endl;
        std::list<Edge<Place *> > &links = vertex->getEdges();
        for (std::list<Edge<Place *> >::iterator it = links.begin(); it != links.end(); it++) {
            Edge<Place *> &edge = *it;
            Vertex<Place *> *linked = edge.vertex;
            std::cout << "\t-> Place " << linked->element->id << "[" << linked << "] with cost " << edge.cost << std::endl;
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
        // Use this flag to check whether anything changed on the iteration
        // If nothing changed, no need to continue as nothing will change in the next step
        bool done = true;

        // For each edge, relax if possible
        for (unsigned int placeIndex = 0; placeIndex < this->placesLength; placeIndex++) {
            Vertex<Place *> *origin = places[placeIndex];
            if (origin != NULL) {
                std::list<Edge<Place *> > &edges = origin->getEdges();
                for (std::list<Edge<Place *> >::iterator it = edges.begin(); it != edges.end(); it++) {
                    Edge<Place *> &edge = *it;
                    Vertex<Place *> *destination = edge.vertex;
                    // Relax whenever possible
                    // Overflow is possible for enormous (near 2^31) weights
                    if (origin->distance != INFINITE && origin->distance + edge.cost < destination->distance) {
                        done = false;
                        destination->distance = origin->distance + edge.cost;
                    }
                }
            }

            if (done){
                break;
            }
        }
    }

    return true;
}

void Graph::dijkstra(Vertex<Place *> *source) {
    // Initialize the graph
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = places[placeIndex];
        vertex->reset();
    }

    // Create the priority queue (reversed)
    std::priority_queue<Vertex<Place*>*, std::vector<Vertex<Place*>*>, distance_less> queue;

    // Insert the source into the queue, with distance 0
    source->distance = 0;
    queue.push(source);

    // Run dijkstra main loop
    while (!queue.empty()){
        // Get top element from the priority queue
        Vertex<Place *> *current = queue.top();
        queue.pop();

        // Iterate through every neighbour
        std::list<Edge<Place *> > &edges = current->getEdges();
        for (std::list<Edge<Place *> >::iterator it = edges.begin(); it != edges.end(); it++) {
            Edge<Place *> &edge = *it;
            Vertex<Place *> *destination = edge.vertex;

            // If this newly discovered distance is shorter than the previous ones
            if (current->distance + edge.cost < destination->distance) {
                destination->distance = current->distance + edge.cost;
                queue.push(destination);
            }
        }
    }


}

void Graph::transpose() {
    // Store the number of edges for each vertex
    // Store it under its distance since it was ruined anyway
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *vertex = places[placeIndex];
        std::list<Edge<Place *> > &edges = vertex->getEdges();
        vertex->distance = edges.size();
    }

    // Iterate each vertex
    for (unsigned int placeIndex = PLACES_START_INDEX; placeIndex < this->placesLength; placeIndex++) {
        Vertex<Place *> *origin = places[placeIndex];
        std::list<Edge<Place *> > &edges = origin->getEdges();

        // Since we know how many edges it has, we can avoid running this twice for the same edge
        while (origin->distance != 0) {
            // Get the edge and create its tranposition
            Edge<Place *> &edge = edges.front();
            Vertex<Place *> *destination = edge.vertex;
            destination->addLink(origin, edge.cost);

            // Destroy the edge and decrease the counter
            edges.pop_front();
            origin->distance--;
        }
    }
}

Graph::~Graph() {
    delete[] this->branches;
    for (unsigned int i = 0; i < this->placesLength; ++i) {
        delete this->places[i];
    }
    delete[] this->places;
}

int main() {
    Graph *graph = new Graph();
    graph->populate();
    graph->execute();
    delete graph;
    return 0;
}
