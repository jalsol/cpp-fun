#include "vector.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("vector default constructor", "[vector][constructor]") {
  jal::vector<int> v;
  REQUIRE(v.empty());
  REQUIRE(v.size() == 0);
  REQUIRE(v.capacity() == 0);
  REQUIRE(v.data() == nullptr);
}

TEST_CASE("vector size constructor", "[vector][constructor]") {
  jal::vector<int> v(5);
  REQUIRE(!v.empty());
  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() == 5);
  REQUIRE(v.data() != nullptr);

  // Default constructed elements
  for (std::size_t i = 0; i < v.size(); ++i) {
    REQUIRE(v[i] == 0);
  }
}

TEST_CASE("vector size and value constructor", "[vector][constructor]") {
  jal::vector<int> v(5, 42);
  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() == 5);

  for (std::size_t i = 0; i < v.size(); ++i) {
    REQUIRE(v[i] == 42);
  }
}

TEST_CASE("vector initializer list constructor", "[vector][constructor]") {
  jal::vector<int> v{1, 2, 3, 4, 5};
  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() == 5);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 2);
  REQUIRE(v[2] == 3);
  REQUIRE(v[3] == 4);
  REQUIRE(v[4] == 5);
}

TEST_CASE("vector copy constructor", "[vector][constructor]") {
  jal::vector<int> v1{1, 2, 3, 4, 5};
  jal::vector<int> v2(v1);

  REQUIRE(v2.size() == v1.size());
  REQUIRE(v2.capacity() == v1.capacity());

  for (std::size_t i = 0; i < v1.size(); ++i) {
    REQUIRE(v2[i] == v1[i]);
  }

  // Verify deep copy
  v2[0] = 99;
  REQUIRE(v1[0] == 1);
}

TEST_CASE("vector move constructor", "[vector][constructor]") {
  jal::vector<int> v1{1, 2, 3, 4, 5};
  const auto *old_data = v1.data();
  const auto old_size = v1.size();
  const auto old_capacity = v1.capacity();

  jal::vector<int> v2(std::move(v1));

  REQUIRE(v2.size() == old_size);
  REQUIRE(v2.capacity() == old_capacity);
  REQUIRE(v2.data() == old_data);

  REQUIRE(v1.size() == 0);
  REQUIRE(v1.capacity() == 0);
  REQUIRE(v1.data() == nullptr);
}

TEST_CASE("vector copy assignment", "[vector][assignment]") {
  jal::vector<int> v1{1, 2, 3, 4, 5};
  jal::vector<int> v2;

  v2 = v1;

  REQUIRE(v2.size() == v1.size());
  for (std::size_t i = 0; i < v1.size(); ++i) {
    REQUIRE(v2[i] == v1[i]);
  }

  // Self-assignment
  v2 = v2;
  REQUIRE(v2.size() == v1.size());
}

TEST_CASE("vector move assignment", "[vector][assignment]") {
  jal::vector<int> v1{1, 2, 3, 4, 5};
  jal::vector<int> v2;

  const auto *old_data = v1.data();
  const auto old_size = v1.size();

  v2 = std::move(v1);

  REQUIRE(v2.size() == old_size);
  REQUIRE(v2.data() == old_data);
  REQUIRE(v1.data() == nullptr);

  // Self-assignment (move)
  v2 = std::move(v2);
  REQUIRE(v2.size() == old_size);
}

TEST_CASE("vector push_back lvalue", "[vector][push_back]") {
  jal::vector<int> v;

  int val1 = 10;
  int val2 = 20;
  int val3 = 30;

  v.push_back(val1);
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == 10);

  v.push_back(val2);
  REQUIRE(v.size() == 2);
  REQUIRE(v[1] == 20);

  v.push_back(val3);
  REQUIRE(v.size() == 3);
  REQUIRE(v[2] == 30);
}

TEST_CASE("vector push_back rvalue", "[vector][push_back]") {
  jal::vector<std::string> v;

  v.push_back("hello");
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == "hello");

  v.push_back("world");
  REQUIRE(v.size() == 2);
  REQUIRE(v[1] == "world");
}

TEST_CASE("vector push_back triggers reallocation", "[vector][push_back]") {
  jal::vector<int> v;

  for (int i = 0; i < 100; ++i) {
    v.push_back(i);
  }

  REQUIRE(v.size() == 100);
  for (int i = 0; i < 100; ++i) {
    REQUIRE(v[i] == i);
  }
}

TEST_CASE("vector emplace_back with simple types", "[vector][emplace_back]") {
  jal::vector<int> v;

  int &ref1 = v.emplace_back(10);
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == 10);
  REQUIRE(&ref1 == &v[0]);

  int &ref2 = v.emplace_back(20);
  REQUIRE(v.size() == 2);
  REQUIRE(v[1] == 20);
  REQUIRE(&ref2 == &v[1]);

  int &ref3 = v.emplace_back(30);
  REQUIRE(v.size() == 3);
  REQUIRE(v[2] == 30);
  REQUIRE(&ref3 == &v[2]);
}

TEST_CASE("vector emplace_back with strings", "[vector][emplace_back]") {
  jal::vector<std::string> v;

  // Construct string in-place from const char*
  std::string &ref1 = v.emplace_back("hello");
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == "hello");
  REQUIRE(&ref1 == &v[0]);

  // Construct string in-place with count and char
  std::string &ref2 = v.emplace_back(5, 'a');
  REQUIRE(v.size() == 2);
  REQUIRE(v[1] == "aaaaa");
  REQUIRE(&ref2 == &v[1]);

  // Construct string in-place from substring
  std::string source = "world";
  std::string &ref3 = v.emplace_back(source.begin(), source.end());
  REQUIRE(v.size() == 3);
  REQUIRE(v[2] == "world");
  REQUIRE(&ref3 == &v[2]);
}

TEST_CASE("vector emplace_back with complex types", "[vector][emplace_back]") {
  struct Point {
    int x, y;
    Point(int x = 0, int y = 0)
        : x(x)
        , y(y) {}
    bool operator==(const Point &other) const {
      return x == other.x && y == other.y;
    }
  };

  jal::vector<Point> v;

  Point &p1 = v.emplace_back(1, 2);
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == Point{1, 2});
  REQUIRE(&p1 == &v[0]);
  REQUIRE(p1.x == 1);
  REQUIRE(p1.y == 2);

  Point &p2 = v.emplace_back(3, 4);
  REQUIRE(v.size() == 2);
  REQUIRE(v[1] == Point{3, 4});
  REQUIRE(&p2 == &v[1]);

  // Test with default arguments
  Point &p3 = v.emplace_back();
  REQUIRE(v.size() == 3);
  REQUIRE(v[2] == Point{0, 0});
  REQUIRE(&p3 == &v[2]);
}

TEST_CASE("vector emplace_back triggers reallocation",
          "[vector][emplace_back]") {
  jal::vector<int> v;

  for (int i = 0; i < 100; ++i) {
    int &ref = v.emplace_back(i);
    REQUIRE(ref == i);
    REQUIRE(&ref == &v[i]);
  }

  REQUIRE(v.size() == 100);
  for (int i = 0; i < 100; ++i) {
    REQUIRE(v[i] == i);
  }
}

TEST_CASE("vector emplace_back returns reference", "[vector][emplace_back]") {
  jal::vector<int> v;

  int &ref = v.emplace_back(42);
  REQUIRE(ref == 42);

  // Modify through reference
  ref = 100;
  REQUIRE(v[0] == 100);
  REQUIRE(v.back() == 100);
}

TEST_CASE("vector pop_back", "[vector][pop_back]") {
  jal::vector<int> v{1, 2, 3, 4, 5};

  v.pop_back();
  REQUIRE(v.size() == 4);
  REQUIRE(v[3] == 4);

  v.pop_back();
  REQUIRE(v.size() == 3);
  REQUIRE(v[2] == 3);
}

TEST_CASE("vector empty", "[vector][empty]") {
  jal::vector<int> v;
  REQUIRE(v.empty());

  v.push_back(1);
  REQUIRE(!v.empty());

  v.pop_back();
  REQUIRE(v.empty());
}

TEST_CASE("vector size and capacity", "[vector][size]") {
  jal::vector<int> v;
  REQUIRE(v.size() == 0);
  REQUIRE(v.capacity() == 0);

  v.push_back(1);
  REQUIRE(v.size() == 1);
  REQUIRE(v.capacity() >= 1);

  v.push_back(2);
  REQUIRE(v.size() == 2);
  REQUIRE(v.capacity() >= 2);
}

TEST_CASE("vector front and back", "[vector][access]") {
  jal::vector<int> v{1, 2, 3, 4, 5};

  REQUIRE(v.front() == 1);
  REQUIRE(v.back() == 5);

  v.front() = 10;
  v.back() = 50;

  REQUIRE(v.front() == 10);
  REQUIRE(v.back() == 50);
  REQUIRE(v[0] == 10);
  REQUIRE(v[4] == 50);
}

TEST_CASE("vector operator[]", "[vector][access]") {
  jal::vector<int> v{1, 2, 3, 4, 5};

  for (std::size_t i = 0; i < v.size(); ++i) {
    REQUIRE(v[i] == static_cast<int>(i + 1));
  }

  v[2] = 99;
  REQUIRE(v[2] == 99);
}

TEST_CASE("vector const operator[]", "[vector][access]") {
  const jal::vector<int> v{1, 2, 3, 4, 5};

  for (std::size_t i = 0; i < v.size(); ++i) {
    REQUIRE(v[i] == static_cast<int>(i + 1));
  }
}

TEST_CASE("vector data", "[vector][data]") {
  jal::vector<int> v{1, 2, 3, 4, 5};

  int *ptr = v.data();
  REQUIRE(ptr != nullptr);
  REQUIRE(*ptr == 1);
  REQUIRE(*(ptr + 4) == 5);

  *ptr = 99;
  REQUIRE(v[0] == 99);
}

TEST_CASE("vector iterators", "[vector][iterator]") {
  jal::vector<int> v{1, 2, 3, 4, 5};

  REQUIRE(v.begin() != v.end());
  REQUIRE(v.end() - v.begin() == 5);

  int sum = 0;
  for (auto it = v.begin(); it != v.end(); ++it) {
    sum += *it;
  }
  REQUIRE(sum == 15);

  // Range-based for loop
  sum = 0;
  for (int val : v) {
    sum += val;
  }
  REQUIRE(sum == 15);
}

TEST_CASE("vector reserve", "[vector][reserve]") {
  jal::vector<int> v;

  v.reserve(10);
  REQUIRE(v.capacity() >= 10);
  REQUIRE(v.size() == 0);

  for (int i = 0; i < 10; ++i) {
    v.push_back(i);
  }
  REQUIRE(v.capacity() >= 10);

  // Reserve smaller capacity should not change capacity
  const auto old_capacity = v.capacity();
  v.reserve(5);
  REQUIRE(v.capacity() == old_capacity);
}

TEST_CASE("vector shrink_to_fit", "[vector][shrink_to_fit]") {
  jal::vector<int> v;
  v.reserve(100);

  for (int i = 0; i < 10; ++i) {
    v.push_back(i);
  }

  REQUIRE(v.capacity() >= 100);
  REQUIRE(v.size() == 10);

  v.shrink_to_fit();
  REQUIRE(v.capacity() == 10);
  REQUIRE(v.size() == 10);

  // Verify elements are preserved
  for (int i = 0; i < 10; ++i) {
    REQUIRE(v[i] == i);
  }
}

TEST_CASE("vector clear", "[vector][clear]") {
  jal::vector<int> v{1, 2, 3, 4, 5};
  const auto old_capacity = v.capacity();

  v.clear();
  REQUIRE(v.size() == 0);
  REQUIRE(v.empty());
  REQUIRE(v.capacity() == old_capacity); // Capacity unchanged

  // Should be able to add elements after clear
  v.push_back(99);
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == 99);
}

TEST_CASE("vector swap", "[vector][swap]") {
  jal::vector<int> v1{1, 2, 3};
  jal::vector<int> v2{4, 5, 6, 7, 8};

  const auto v1_size = v1.size();
  const auto v2_size = v2.size();
  const auto v1_data = v1.data();
  const auto v2_data = v2.data();

  v1.swap(v2);

  REQUIRE(v1.size() == v2_size);
  REQUIRE(v2.size() == v1_size);
  REQUIRE(v1.data() == v2_data);
  REQUIRE(v2.data() == v1_data);

  REQUIRE(v1[0] == 4);
  REQUIRE(v2[0] == 1);
}

TEST_CASE("vector with strings", "[vector][string]") {
  jal::vector<std::string> v;

  v.push_back("hello");
  v.push_back("world");
  v.push_back("test");

  REQUIRE(v.size() == 3);
  REQUIRE(v[0] == "hello");
  REQUIRE(v[1] == "world");
  REQUIRE(v[2] == "test");

  jal::vector<std::string> v2 = v;
  REQUIRE(v2.size() == 3);
  REQUIRE(v2[0] == "hello");
}

TEST_CASE("vector with complex types", "[vector][complex]") {
  struct Point {
    int x, y;
    Point(int x = 0, int y = 0)
        : x(x)
        , y(y) {}
    bool operator==(const Point &other) const {
      return x == other.x && y == other.y;
    }
  };

  jal::vector<Point> v;
  v.push_back(Point{1, 2});
  v.push_back(Point{3, 4});

  REQUIRE(v.size() == 2);
  REQUIRE(v[0] == Point{1, 2});
  REQUIRE(v[1] == Point{3, 4});
}

TEST_CASE("vector zero-size constructor", "[vector][constructor]") {
  jal::vector<int> v(0);
  REQUIRE(v.empty());
  REQUIRE(v.size() == 0);
}

TEST_CASE("vector empty initializer list", "[vector][constructor]") {
  jal::vector<int> v{};
  REQUIRE(v.empty());
  REQUIRE(v.size() == 0);
}
