#include "../src/aabb.h"
#include "../tests/test_framework.h"
#include <cmath>

// Forward declarations
bool test_aabb_default_constructor();
bool test_aabb_clear();
bool test_aabb_expand_by_point();
bool test_aabb_expand_by_aabb();
bool test_aabb_contains();
bool test_aabb_largest_axis();
bool test_aabb_surface_area();
bool test_aabb_center();
bool test_aabb_merge();
bool test_aabb_squared_distance();
bool test_ray_constructor();
bool test_ray_evaluate();
bool test_aabb_ray_intersect_hit();
bool test_aabb_ray_intersect_miss();

int main() {
    std::cout << "=== AABB and Ray Tests ===" << std::endl;

    int passed = 0, failed = 0;

    RUN_TEST(aabb_default_constructor);
    RUN_TEST(aabb_clear);
    RUN_TEST(aabb_expand_by_point);
    RUN_TEST(aabb_expand_by_aabb);
    RUN_TEST(aabb_contains);
    RUN_TEST(aabb_largest_axis);
    RUN_TEST(aabb_surface_area);
    RUN_TEST(aabb_center);
    RUN_TEST(aabb_merge);
    RUN_TEST(aabb_squared_distance);
    RUN_TEST(ray_constructor);
    RUN_TEST(ray_evaluate);
    RUN_TEST(aabb_ray_intersect_hit);
    RUN_TEST(aabb_ray_intersect_miss);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}

// Test implementations
bool test_aabb_default_constructor() {
    AABB aabb;
    ASSERT_TRUE(std::isinf(aabb.min.x()) && std::isinf(aabb.min.y()) && std::isinf(aabb.min.z()));
    ASSERT_TRUE(std::isinf(aabb.max.x()) && std::isinf(aabb.max.y()) && std::isinf(aabb.max.z()));
    return true;
}

bool test_aabb_clear() {
    AABB aabb;
    aabb.expandBy(Vector3f(1.0f, 2.0f, 3.0f));
    aabb.clear();
    ASSERT_TRUE(std::isinf(aabb.min.x()) && std::isinf(aabb.min.y()) && std::isinf(aabb.min.z()));
    return true;
}

bool test_aabb_expand_by_point() {
    AABB aabb;
    aabb.expandBy(Vector3f(1.0f, 2.0f, 3.0f));
    aabb.expandBy(Vector3f(5.0f, 6.0f, 7.0f));

    ASSERT_FLOAT_EQ(aabb.min.x(), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.min.y(), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.min.z(), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.max.x(), 5.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.max.y(), 6.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.max.z(), 7.0f, 1e-6f);
    return true;
}

bool test_aabb_expand_by_aabb() {
    AABB aabb1, aabb2;
    aabb1.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb1.expandBy(Vector3f(2.0f, 2.0f, 2.0f));
    aabb2.expandBy(Vector3f(1.0f, 1.0f, 1.0f));
    aabb2.expandBy(Vector3f(3.0f, 3.0f, 3.0f));

    aabb1.expandBy(aabb2);

    ASSERT_FLOAT_EQ(aabb1.min.x(), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb1.max.x(), 3.0f, 1e-6f);
    return true;
}

bool test_aabb_contains() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(4.0f, 4.0f, 4.0f));

    ASSERT_TRUE(aabb.contains(Vector3f(2.0f, 2.0f, 2.0f)));
    ASSERT_TRUE(aabb.contains(Vector3f(0.0f, 0.0f, 0.0f)));
    ASSERT_TRUE(aabb.contains(Vector3f(4.0f, 4.0f, 4.0f)));
    ASSERT_FALSE(aabb.contains(Vector3f(-1.0f, 2.0f, 2.0f)));
    ASSERT_FALSE(aabb.contains(Vector3f(5.0f, 2.0f, 2.0f)));
    return true;
}

bool test_aabb_largest_axis() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(10.0f, 5.0f, 3.0f));

    ASSERT_EQ(aabb.largestAxis(), 0);

    aabb.clear();
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(3.0f, 10.0f, 5.0f));
    ASSERT_EQ(aabb.largestAxis(), 1);

    aabb.clear();
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(3.0f, 5.0f, 10.0f));
    ASSERT_EQ(aabb.largestAxis(), 2);
    return true;
}

bool test_aabb_surface_area() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(2.0f, 3.0f, 4.0f));

    Float expected = 2.0f * (2.0f*3.0f + 2.0f*4.0f + 3.0f*4.0f);
    ASSERT_FLOAT_EQ(aabb.surfaceArea(), expected, 1e-6f);
    return true;
}

bool test_aabb_center() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(4.0f, 6.0f, 8.0f));

    Vector3f c = aabb.center();
    ASSERT_FLOAT_EQ(c.x(), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(c.y(), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(c.z(), 4.0f, 1e-6f);
    return true;
}

bool test_aabb_merge() {
    AABB aabb1, aabb2;
    aabb1.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb1.expandBy(Vector3f(2.0f, 2.0f, 2.0f));
    aabb2.expandBy(Vector3f(1.0f, 1.0f, 1.0f));
    aabb2.expandBy(Vector3f(3.0f, 3.0f, 3.0f));

    AABB merged = AABB::merge(aabb1, aabb2);

    ASSERT_FLOAT_EQ(merged.min.x(), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(merged.max.x(), 3.0f, 1e-6f);
    return true;
}

bool test_aabb_squared_distance() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(2.0f, 2.0f, 2.0f));

    ASSERT_FLOAT_EQ(aabb.squaredDistanceTo(Vector3f(1.0f, 1.0f, 1.0f)), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.squaredDistanceTo(Vector3f(-1.0f, 1.0f, 1.0f)), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(aabb.squaredDistanceTo(Vector3f(3.0f, 3.0f, 3.0f)), 3.0f, 1e-6f);
    return true;
}

bool test_ray_constructor() {
    Ray ray(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f));

    ASSERT_FLOAT_EQ(ray.o.x(), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(ray.d.x(), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(ray.mint, 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(ray.maxt, std::numeric_limits<Float>::infinity(), 1e-6f);
    return true;
}

bool test_ray_evaluate() {
    Ray ray(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 2.0f, 3.0f));

    Vector3f p = ray(2.0f);
    ASSERT_FLOAT_EQ(p.x(), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(p.y(), 4.0f, 1e-6f);
    ASSERT_FLOAT_EQ(p.z(), 6.0f, 1e-6f);
    return true;
}

bool test_aabb_ray_intersect_hit() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(4.0f, 4.0f, 4.0f));

    Ray ray(Vector3f(-1.0f, 2.0f, 2.0f), Vector3f(1.0f, 0.0f, 0.0f));

    ASSERT_TRUE(aabb.rayIntersect(ray));
    return true;
}

bool test_aabb_ray_intersect_miss() {
    AABB aabb;
    aabb.expandBy(Vector3f(0.0f, 0.0f, 0.0f));
    aabb.expandBy(Vector3f(4.0f, 4.0f, 4.0f));

    Ray ray(Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(1.0f, 0.0f, 0.0f));

    ASSERT_FALSE(aabb.rayIntersect(ray));
    return true;
}