#include "../src/serializer.h"
#include "../tests/test_framework.h"

// Forward declarations
bool test_serializer_default_constructor();
bool test_serializer_set_get_uint32();
bool test_serializer_set_get_float();
bool test_serializer_set_get_double();
bool test_serializer_set_get_bool();
bool test_serializer_set_get_vector2f();
bool test_serializer_set_get_vector3f();
bool test_serializer_set_get_vector4f();
bool test_serializer_set_get_vectorxf();
bool test_serializer_set_get_matrix();
bool test_serializer_prefix_stack();
bool test_serializer_set_get_string();
bool test_serializer_map_operations();
bool test_serializer_set_operations();
bool test_serializer_missing_key();

int main() {
    std::cout << "=== Serializer Tests ===" << std::endl;

    int passed = 0, failed = 0;

    RUN_TEST(serializer_default_constructor);
    RUN_TEST(serializer_set_get_uint32);
    RUN_TEST(serializer_set_get_float);
    RUN_TEST(serializer_set_get_double);
    RUN_TEST(serializer_set_get_bool);
    RUN_TEST(serializer_set_get_vector2f);
    RUN_TEST(serializer_set_get_vector3f);
    RUN_TEST(serializer_set_get_vector4f);
    RUN_TEST(serializer_set_get_vectorxf);
    RUN_TEST(serializer_set_get_matrix);
    RUN_TEST(serializer_prefix_stack);
    RUN_TEST(serializer_set_get_string);
    RUN_TEST(serializer_map_operations);
    RUN_TEST(serializer_set_operations);
    RUN_TEST(serializer_missing_key);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}

bool test_serializer_default_constructor() {
    Serializer s;
    ASSERT_EQ(s.totalSize(), 0u);
    return true;
}

bool test_serializer_set_get_uint32() {
    Serializer s;
    uint32_t val = 42;
    s.set("test", val);

    uint32_t retrieved = 0;
    bool found = s.get("test", retrieved);

    ASSERT_TRUE(found);
    ASSERT_EQ(retrieved, 42);
    return true;
}

bool test_serializer_set_get_float() {
    Serializer s;
    float val = 3.14159f;
    s.set("pi", val);

    float retrieved = 0.0f;
    bool found = s.get("pi", retrieved);

    ASSERT_TRUE(found);
    ASSERT_FLOAT_EQ(retrieved, 3.14159f, 1e-5f);
    return true;
}

bool test_serializer_set_get_double() {
    Serializer s;
    double val = 2.718281828;
    s.set("e", val);

    double retrieved = 0.0;
    bool found = s.get("e", retrieved);

    ASSERT_TRUE(found);
    ASSERT_FLOAT_EQ(retrieved, 2.718281828, 1e-9);
    return true;
}

bool test_serializer_set_get_bool() {
    Serializer s;
    s.set("flag", true);

    bool retrieved = false;
    bool found = s.get("flag", retrieved);

    ASSERT_TRUE(found);
    ASSERT_TRUE(retrieved);

    s.set("flag", false);
    found = s.get("flag", retrieved);
    ASSERT_TRUE(found);
    ASSERT_FALSE(retrieved);
    return true;
}


bool test_serializer_set_get_vector2f() {
    Serializer s;
    Vector2f val(1.5f, 2.5f);
    s.set("vec2", val);

    Vector2f retrieved;
    bool found = s.get("vec2", retrieved);

    ASSERT_TRUE(found);
    ASSERT_FLOAT_EQ(retrieved.x(), 1.5f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved.y(), 2.5f, 1e-6f);
    return true;
}

bool test_serializer_set_get_vector3f() {
    Serializer s;
    Vector3f val(1.0f, 2.0f, 3.0f);
    s.set("vec3", val);

    Vector3f retrieved;
    bool found = s.get("vec3", retrieved);

    ASSERT_TRUE(found);
    ASSERT_FLOAT_EQ(retrieved.x(), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved.y(), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved.z(), 3.0f, 1e-6f);
    return true;
}

bool test_serializer_set_get_vector4f() {
    Serializer s;
    Vector4f val(1.0f, 2.0f, 3.0f, 4.0f);
    s.set("vec4", val);

    Vector4f retrieved;
    bool found = s.get("vec4", retrieved);

    ASSERT_TRUE(found);
    ASSERT_FLOAT_EQ(retrieved.x(), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved.y(), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved.z(), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved.w(), 4.0f, 1e-6f);
    return true;
}

bool test_serializer_set_get_vectorxf() {
    Serializer s;
    VectorXf val(5);
    val << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f;
    s.set("vecx", val);

    VectorXf retrieved;
    bool found = s.get("vecx", retrieved);

    ASSERT_TRUE(found);
    ASSERT_EQ(retrieved.size(), 5);
    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(retrieved(i), (float)(i + 1), 1e-6f);
    }
    return true;
}

bool test_serializer_set_get_matrix() {
    Serializer s;
    MatrixXf val(2, 3);
    val << 1.0f, 2.0f, 3.0f,
           4.0f, 5.0f, 6.0f;
    s.set("matrix", val);

    MatrixXf retrieved;
    bool found = s.get("matrix", retrieved);

    ASSERT_TRUE(found);
    ASSERT_EQ(retrieved.rows(), 2);
    ASSERT_EQ(retrieved.cols(), 3);
    ASSERT_FLOAT_EQ(retrieved(0, 0), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved(1, 2), 6.0f, 1e-6f);
    return true;
}

bool test_serializer_prefix_stack() {
    Serializer s;
    s.set("value1", 1);
    s.pushPrefix("section");
    s.set("value2", 2);

    int v1 = 0, v2 = 0;
    // value1 is outside the section, but Serializer finds it if we don't have the prefix pushed
    // When prefix is pushed, we can't find value1 unless we pop or use absolute path if supported (it's not).
    
    bool found2 = s.get("value2", v2);
    ASSERT_TRUE(found2);
    ASSERT_EQ(v2, 2);

    s.popPrefix();
    bool found1 = s.get("value1", v1);
    ASSERT_TRUE(found1);
    ASSERT_EQ(v1, 1);
    
    bool found2_post = s.get("section.value2", v2);
    ASSERT_TRUE(found2_post);
    ASSERT_EQ(v2, 2);

    return true;
}

bool test_serializer_set_get_string() {
    Serializer s;
    std::string val = "hello world";
    s.set("message", val);

    std::string retrieved;
    bool found = s.get("message", retrieved);

    ASSERT_TRUE(found);
    ASSERT_EQ(retrieved, "hello world");
    return true;
}

bool test_serializer_map_operations() {
    Serializer s;
    std::map<uint32_t, float> val;
    val[1] = 1.5f;
    val[2] = 2.5f;
    val[3] = 3.5f;
    s.set("map", val);

    std::map<uint32_t, float> retrieved;
    bool found = s.get("map", retrieved);

    ASSERT_TRUE(found);
    ASSERT_EQ(retrieved.size(), 3);
    ASSERT_FLOAT_EQ(retrieved[1], 1.5f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved[2], 2.5f, 1e-6f);
    ASSERT_FLOAT_EQ(retrieved[3], 3.5f, 1e-6f);
    return true;
}

bool test_serializer_set_operations() {
    Serializer s;
    std::set<uint32_t> val;
    val.insert(5);
    val.insert(10);
    val.insert(15);
    s.set("set", val);

    std::set<uint32_t> retrieved;
    bool found = s.get("set", retrieved);

    ASSERT_TRUE(found);
    ASSERT_EQ(retrieved.size(), 3);
    ASSERT_TRUE(retrieved.count(5) > 0);
    ASSERT_TRUE(retrieved.count(10) > 0);
    ASSERT_TRUE(retrieved.count(15) > 0);
    return true;
}

bool test_serializer_missing_key() {
    Serializer s;
    s.set("existing", 42);

    int value = 0;
    bool found = false;
    try {
        found = s.get("nonexistent", value);
    } catch (const std::runtime_error &) {
        found = false;
    }

    ASSERT_FALSE(found);
    ASSERT_EQ(value, 0);
    return true;
}