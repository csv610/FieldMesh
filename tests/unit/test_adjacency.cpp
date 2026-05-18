#include "../src/adjacency.h"
#include "../tests/test_framework.h"

// Forward declarations
bool test_link_default_constructor();
bool test_link_constructor_with_id();
bool test_link_constructor_with_id_and_weight();
bool test_link_comparison();
bool test_integer_variable_shift();
bool test_integer_variable_set_shift();
bool test_adjacency_matrix_default_constructor();
bool test_adjacency_matrix_nullptr_constructor();
bool test_adjacency_matrix_sized_constructor();
bool test_adjacency_matrix_move_constructor();
bool test_adjacency_matrix_move_assignment();
bool test_adjacency_matrix_index_operator();
bool test_adjacency_matrix_bool_conversion();
bool test_adjacency_matrix_data_access();

int main() {
    std::cout << "=== Adjacency Matrix Tests ===" << std::endl;

    int passed = 0, failed = 0;

    RUN_TEST(link_default_constructor);
    RUN_TEST(link_constructor_with_id);
    RUN_TEST(link_constructor_with_id_and_weight);
    RUN_TEST(link_comparison);
    RUN_TEST(integer_variable_shift);
    RUN_TEST(integer_variable_set_shift);
    RUN_TEST(adjacency_matrix_default_constructor);
    RUN_TEST(adjacency_matrix_nullptr_constructor);
    RUN_TEST(adjacency_matrix_sized_constructor);
    RUN_TEST(adjacency_matrix_move_constructor);
    RUN_TEST(adjacency_matrix_move_assignment);
    RUN_TEST(adjacency_matrix_index_operator);
    RUN_TEST(adjacency_matrix_bool_conversion);
    RUN_TEST(adjacency_matrix_data_access);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}

bool test_link_default_constructor() {
    Link link;
    ASSERT_EQ(link.id, 0);
    ASSERT_FLOAT_EQ(link.weight, 1.0f, 1e-6f);
    return true;
}

bool test_link_constructor_with_id() {
    Link link(5);
    ASSERT_EQ(link.id, 5);
    ASSERT_FLOAT_EQ(link.weight, 1.0f, 1e-6f);
    return true;
}

bool test_link_constructor_with_id_and_weight() {
    Link link(3, 0.5f);
    ASSERT_EQ(link.id, 3);
    ASSERT_FLOAT_EQ(link.weight, 0.5f, 1e-6f);
    return true;
}

bool test_link_comparison() {
    Link link1(3), link2(5);
    ASSERT_TRUE(link1 < link2);
    ASSERT_FALSE(link2 < link1);
    return true;
}

bool test_integer_variable_shift() {
    IntegerVariable iv;
    iv.translate_u = 3;
    iv.translate_v = -2;

    Vector2i shift = iv.shift();
    ASSERT_EQ(shift.x(), 3);
    ASSERT_EQ(shift.y(), -2);
    return true;
}

bool test_integer_variable_set_shift() {
    IntegerVariable iv;
    iv.setShift(Vector2i(5, -3));

    ASSERT_EQ(iv.translate_u, 5);
    ASSERT_EQ(iv.translate_v, -3);
    return true;
}

bool test_adjacency_matrix_default_constructor() {
    AdjacencyMatrix adj;
    ASSERT_FALSE(adj);
    return true;
}

bool test_adjacency_matrix_nullptr_constructor() {
    AdjacencyMatrix adj = nullptr;
    ASSERT_FALSE(adj);
    return true;
}

bool test_adjacency_matrix_sized_constructor() {
    AdjacencyMatrix adj(4, 10);
    ASSERT_TRUE(adj);
    ASSERT_NE(adj.get(), nullptr);
    return true;
}

bool test_adjacency_matrix_move_constructor() {
    AdjacencyMatrix adj1(4, 10);
    Link** rows = adj1.get();

    AdjacencyMatrix adj2(std::move(adj1));
    ASSERT_TRUE(adj2);
    ASSERT_EQ(adj2.get(), rows);
    ASSERT_FALSE(adj1);
    return true;
}

bool test_adjacency_matrix_move_assignment() {
    AdjacencyMatrix adj1(4, 10);
    Link** rows = adj1.get();

    AdjacencyMatrix adj2;
    adj2 = std::move(adj1);
    ASSERT_TRUE(adj2);
    ASSERT_EQ(adj2.get(), rows);
    return true;
}

bool test_adjacency_matrix_index_operator() {
    AdjacencyMatrix adj(4, 6);

    Link** rows = adj.get();
    rows[0] = adj.data();
    rows[1] = adj.data() + 2;
    rows[2] = adj.data() + 4;
    rows[3] = adj.data() + 6;
    rows[4] = nullptr;

    ASSERT_EQ(adj[0], adj.data());
    ASSERT_EQ(adj[1], adj.data() + 2);
    ASSERT_EQ(adj[4], nullptr);
    return true;
}

bool test_adjacency_matrix_bool_conversion() {
    AdjacencyMatrix empty;
    AdjacencyMatrix sized(4, 10);

    ASSERT_FALSE(empty);
    ASSERT_TRUE(sized);
    return true;
}

bool test_adjacency_matrix_data_access() {
    AdjacencyMatrix adj(2, 3);

    Link* data = adj.data();
    data[0] = Link(1, 0.5f);
    data[1] = Link(2, 0.3f);
    data[2] = Link(3, 0.7f);

    ASSERT_EQ(adj.data()[0].id, 1);
    ASSERT_EQ(adj.data()[1].id, 2);
    ASSERT_EQ(adj.data()[2].id, 3);
    return true;
}