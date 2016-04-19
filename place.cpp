#include <ostream>
#include "place.hpp"

Place::Place(unsigned int id) {
    this->id = id;
    this->branch = NULL;
}

Place::~Place() {
    delete this->branch;
}