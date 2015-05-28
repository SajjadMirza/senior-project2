#ifndef _BOUNDING_BOX
#define _BOUNDING_BOX

#include <common.hpp>
#include <basic_geometry.hpp>

struct BoundingBox {
    Box box;
    BoundingBox();
    BoundingBox(const std::vector< Eigen::Vector3f > &vertices);
    ~BoundingBox();

    bool contains(Eigen::Vector3f point) const;
};

#endif
