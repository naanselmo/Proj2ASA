#ifndef PLACE_H
#define PLACE_H

#include "branch.hpp"

class Place {
public:
    unsigned int id;
    Branch *branch;

    Place(unsigned int id, Branch *const branch);
    Place(unsigned int id);
    virtual ~Place();
};


#endif //PLACE_H
