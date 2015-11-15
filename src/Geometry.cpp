#include <Geometry.hpp>
#include <cmath>
#include <log.hpp>

float classify_point(const Plane &plane, const Eigen::Vector3f &point)
{
    return plane.a * point(0) + plane.b * point(1) + plane.c * point(2) + plane.d;
}

Containment check_frustum_point(const Frustum &frustum, const Eigen::Vector3f &center)
{
    Containment result = INSIDE;
    float left_dist = classify_point(frustum.left, center);
    LOG("left dist " << left_dist);
    if (left_dist < 0) {
        return OUTSIDE;
    }
  
    
    float right_dist = classify_point(frustum.right, center);
    LOG("right dist " << right_dist);
    if (right_dist < 0) {
        return OUTSIDE;
    }


    float top_dist = classify_point(frustum.top, center);
    LOG("top dist " << top_dist);
    if (top_dist < 0) {
        return OUTSIDE;
    }



    float bottom_dist = classify_point(frustum.bottom, center);
    LOG("bottom dist " << bottom_dist);
    if (bottom_dist < 0) {
        return OUTSIDE;
    }


    float far_dist = classify_point(frustum.far, center);
    LOG("far dist " << far_dist);
    if (far_dist < 0) {
        return OUTSIDE;
    }


    float near_dist = classify_point(frustum.near, center);
    LOG("near dist " << near_dist);
    if (near_dist < 0) {
        return OUTSIDE;
    }

    return result;
}

Containment check_frustum_sphere(const Frustum &frustum, const Eigen::Vector3f &center, float radius)
{
    Containment result = INSIDE;
    float left_dist = classify_point(frustum.left, center);
    LOG("left dist " << left_dist);
    if (left_dist < -radius) {
        return OUTSIDE;
    }
    else if (left_dist < radius) {
        result =  INTERSECT;
    }
    
    float right_dist = classify_point(frustum.right, center);
    LOG("right dist " << right_dist);
    if (right_dist < -radius) {
        return OUTSIDE;
    }
    else if (right_dist < radius) {
        result =  INTERSECT;
    }

    float top_dist = classify_point(frustum.top, center);
    LOG("top dist " << top_dist);
    if (top_dist < -radius) {
        return OUTSIDE;
    }
    else if (top_dist < radius) {
        result =  INTERSECT;
    }

    float bottom_dist = classify_point(frustum.bottom, center);
    LOG("bottom dist " << bottom_dist);
    if (bottom_dist < -radius) {
        return OUTSIDE;
    }
    else if (bottom_dist < radius) {
        result =  INTERSECT;
    }

    float far_dist = classify_point(frustum.far, center);
    LOG("far dist " << far_dist);
    if (far_dist < -radius) {
        return OUTSIDE;
    }
    else if (far_dist < radius) {
        result =  INTERSECT;
    }

    float near_dist = classify_point(frustum.near, center);
    LOG("near dist " << near_dist);
    if (near_dist < -radius) {
        return OUTSIDE;
    }
    else if (near_dist < radius) {
        result =  INTERSECT;
    }

    return INSIDE;
}

void normalize_plane(Plane *plane)
{
    Plane &p = *plane;
    float mag = std::sqrt(p.a*p.a + p.b*p.b + p.c*p.c);
    p.a = p.a / mag;
    p.b = p.b / mag;
    p.c = p.c / mag;
    p.d = p.d / mag;
}

void normalize(Frustum *frustum)
{
    normalize_plane(&frustum->left);
    normalize_plane(&frustum->right);
    normalize_plane(&frustum->top);
    normalize_plane(&frustum->bottom);
    normalize_plane(&frustum->near);
    normalize_plane(&frustum->far );
}

void extract_planes(Frustum *frustum, const Eigen::Matrix4f &matrix)
{
    const Eigen::Matrix4f &m = matrix;
    Frustum &f = *frustum;
    // Left
    f.left.a = m(0,3) + m(0,0);
    f.left.b = m(1,3) + m(1,0);
    f.left.c = m(2,3) + m(2,0);
    f.left.d = m(3,3) + m(3,0);
    // Right
    f.right.a = m(0,3) - m(0,0);
    f.right.b = m(1,3) - m(1,0);
    f.right.c = m(2,3) - m(2,0);
    f.right.d = m(3,3) - m(3,0);
    // Top
    f.top.a = m(0,3) - m(0,1);
    f.top.b = m(1,3) - m(1,1);
    f.top.c = m(2,3) - m(2,1);
    f.top.d = m(3,3) - m(3,1);
    // Bottom
    f.bottom.a = m(0,3) + m(0,1);
    f.bottom.b = m(1,3) + m(1,1);
    f.bottom.c = m(2,3) + m(2,1);
    f.bottom.d = m(3,3) + m(3,1);
    // Near
    f.near.a = m(0,3) + m(0,2);
    f.near.b = m(1,3) + m(1,2);
    f.near.c = m(2,3) + m(2,2);
    f.near.d = m(3,3) + m(3,2);
    // Far
    f.far.a = m(0,3) - m(0,2);
    f.far.b = m(1,3) - m(1,2);
    f.far.c = m(2,3) - m(2,2);
    f.far.d = m(3,3) - m(3,2);
}
