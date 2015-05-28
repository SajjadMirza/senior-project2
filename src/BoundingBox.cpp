#include <BoundingBox.hpp>
#include <algorithm>
#include <limits>

BoundingBox::BoundingBox() {
}

BoundingBox::BoundingBox(const std::vector< Eigen::Vector3f > &v) {
    float xmin, ymin, zmin;
    float xmax, ymax, zmax;
    xmin = ymin = zmin = std::numeric_limits<float>::infinity();
    xmax = ymax = zmax = -std::numeric_limits<float>::infinity();

    for (auto it = v.begin(); it != v.end(); it++) {
        Eigen::Vector3f v = *it;
        xmin = std::min(xmin, v(0));
        xmax = std::max(xmax, v(0));
        ymin = std::min(ymin, v(1));
        ymax = std::max(ymax, v(1));
        zmin = std::min(zmin, v(2));
        zmax = std::max(zmax, v(2));
    }

    box.min = Eigen::Vector3f(xmin, ymin, zmin);
    box.max = Eigen::Vector3f(xmax, ymax, zmax);
}

BoundingBox::~BoundingBox() {
}

bool BoundingBox::contains(Eigen::Vector3f point) const {
    bool containsx = box.min(0) <= point(0) && point(0) <= box.max(0);
    bool containsy = box.min(1) <= point(1) && point(1) <= box.max(1);
    bool containsz = box.min(2) <= point(2) && point(2) <= box.max(2);

    return containsx && containsy && containsz;
}
