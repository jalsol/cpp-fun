#include "ranges/filter.h"
#include "ranges/take.h"
#include "ranges/to.h"
#include "ranges/transform.h"
#include <catch2/catch_test_macros.hpp>
#include <deque>
#include <list>
#include <ranges>
#include <set>
#include <string>
#include <vector>

TEST_CASE("transform basic usage", "[ranges][transform]") {
  std::vector<int> input{1, 2, 3, 4, 5};

  auto result = input | jal::ranges::transform([](int x) { return x * 2; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
  REQUIRE(result[3] == 8);
  REQUIRE(result[4] == 10);
}

TEST_CASE("transform with strings", "[ranges][transform]") {
  std::vector<std::string> input{"hello", "world", "test"};

  auto result =
    input |
    jal::ranges::transform([](const std::string &s) { return s.length(); }) |
    jal::ranges::to<std::vector<std::size_t>>();

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 5);
  REQUIRE(result[1] == 5);
  REQUIRE(result[2] == 4);
}

TEST_CASE("filter basic usage", "[ranges][filter]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = input | jal::ranges::filter([](int x) { return x % 2 == 0; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
  REQUIRE(result[3] == 8);
  REQUIRE(result[4] == 10);
}

TEST_CASE("filter removes all elements", "[ranges][filter]") {
  std::vector<int> input{1, 3, 5, 7, 9};

  auto result = input | jal::ranges::filter([](int x) { return x % 2 == 0; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.empty());
}

TEST_CASE("filter keeps all elements", "[ranges][filter]") {
  std::vector<int> input{2, 4, 6, 8};

  auto result = input | jal::ranges::filter([](int x) { return x % 2 == 0; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 4);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
  REQUIRE(result[3] == 8);
}

TEST_CASE("take basic usage", "[ranges][take]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result =
    input | jal::ranges::take(5) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 3);
  REQUIRE(result[3] == 4);
  REQUIRE(result[4] == 5);
}

TEST_CASE("take more than available", "[ranges][take]") {
  std::vector<int> input{1, 2, 3};

  auto result =
    input | jal::ranges::take(10) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 3);
}

TEST_CASE("take zero elements", "[ranges][take]") {
  std::vector<int> input{1, 2, 3, 4, 5};

  auto result =
    input | jal::ranges::take(0) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.empty());
}

TEST_CASE("take from empty range", "[ranges][take]") {
  std::vector<int> input{};

  auto result =
    input | jal::ranges::take(5) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.empty());
}

TEST_CASE("combined transform and filter", "[ranges][combined]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = input | jal::ranges::transform([](int x) { return x * 2; }) |
                jal::ranges::filter([](int x) { return x > 10; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 12);
  REQUIRE(result[1] == 14);
  REQUIRE(result[2] == 16);
  REQUIRE(result[3] == 18);
  REQUIRE(result[4] == 20);
}

TEST_CASE("combined filter and transform", "[ranges][combined]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = input | jal::ranges::filter([](int x) { return x % 2 == 0; }) |
                jal::ranges::transform([](int x) { return x * x; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 4);
  REQUIRE(result[1] == 16);
  REQUIRE(result[2] == 36);
  REQUIRE(result[3] == 64);
  REQUIRE(result[4] == 100);
}

TEST_CASE("combined take and transform", "[ranges][combined]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = input | jal::ranges::take(5) |
                jal::ranges::transform([](int x) { return x * 10; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 10);
  REQUIRE(result[1] == 20);
  REQUIRE(result[2] == 30);
  REQUIRE(result[3] == 40);
  REQUIRE(result[4] == 50);
}

TEST_CASE("combined take and filter", "[ranges][combined]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = input | jal::ranges::take(7) |
                jal::ranges::filter([](int x) { return x % 2 == 0; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
}

TEST_CASE("three-way combination", "[ranges][combined]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  auto result = input | jal::ranges::filter([](int x) { return x % 2 == 1; }) |
                jal::ranges::transform([](int x) { return x * 3; }) |
                jal::ranges::take(5) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 3);
  REQUIRE(result[1] == 9);
  REQUIRE(result[2] == 15);
  REQUIRE(result[3] == 21);
  REQUIRE(result[4] == 27);
}

TEST_CASE("to with different container types", "[ranges][to]") {
  std::vector<int> input{3, 1, 4, 1, 5, 9, 2, 6};

  auto result = input | jal::ranges::to<std::set<int>>();

  REQUIRE(result.size() == 7);
  REQUIRE(result.count(1) == 1);
  REQUIRE(result.count(2) == 1);
  REQUIRE(result.count(3) == 1);
  REQUIRE(result.count(4) == 1);
  REQUIRE(result.count(5) == 1);
  REQUIRE(result.count(6) == 1);
  REQUIRE(result.count(9) == 1);
}

TEST_CASE("transform without materialization", "[ranges][lazy]") {
  std::vector<int> input{1, 2, 3, 4, 5};

  auto view = input | jal::ranges::transform([](int x) { return x * 2; });

  int sum = 0;
  for (auto val : view) {
    sum += val;
  }

  REQUIRE(sum == 30);
}

TEST_CASE("filter without materialization", "[ranges][lazy]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto view = input | jal::ranges::filter([](int x) { return x % 2 == 0; });

  int count = 0;
  for (auto val : view) {
    count++;
    (void)val;
  }

  REQUIRE(count == 5);
}

TEST_CASE("take without materialization", "[ranges][lazy]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto view = input | jal::ranges::take(3);

  std::vector<int> result;
  for (auto val : view) {
    result.push_back(val);
  }

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 3);
}

TEST_CASE("empty input range", "[ranges][edge]") {
  std::vector<int> input{};

  auto result = input | jal::ranges::transform([](int x) { return x * 2; }) |
                jal::ranges::filter([](int x) { return x > 0; }) |
                jal::ranges::take(10) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.empty());
}

TEST_CASE("complex transformation pipeline", "[ranges][complex]") {
  std::vector<int> input{-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};

  auto result = input | jal::ranges::filter([](int x) { return x >= 0; }) |
                jal::ranges::transform([](int x) { return x * x; }) |
                jal::ranges::filter([](int x) { return x < 20; }) |
                jal::ranges::take(4) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 4);
  REQUIRE(result[0] == 0);
  REQUIRE(result[1] == 1);
  REQUIRE(result[2] == 4);
  REQUIRE(result[3] == 9);
}

TEST_CASE("iota basic usage", "[ranges][iota]") {
  auto result = std::views::iota(1, 6) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 3);
  REQUIRE(result[3] == 4);
  REQUIRE(result[4] == 5);
}

TEST_CASE("iota with transform", "[ranges][iota]") {
  auto result = std::views::iota(1, 11) |
                jal::ranges::transform([](int x) { return x * x; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 10);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 9);
  REQUIRE(result[3] == 16);
  REQUIRE(result[4] == 25);
  REQUIRE(result[5] == 36);
  REQUIRE(result[6] == 49);
  REQUIRE(result[7] == 64);
  REQUIRE(result[8] == 81);
  REQUIRE(result[9] == 100);
}

TEST_CASE("iota with filter", "[ranges][iota]") {
  auto result = std::views::iota(1, 21) |
                jal::ranges::filter([](int x) { return x % 2 == 0; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 10);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
  REQUIRE(result[3] == 8);
  REQUIRE(result[4] == 10);
}

TEST_CASE("iota with take", "[ranges][iota]") {
  auto result = std::views::iota(0, 100) | jal::ranges::take(7) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 7);
  REQUIRE(result[0] == 0);
  REQUIRE(result[1] == 1);
  REQUIRE(result[2] == 2);
  REQUIRE(result[3] == 3);
  REQUIRE(result[4] == 4);
  REQUIRE(result[5] == 5);
  REQUIRE(result[6] == 6);
}

TEST_CASE("iota unbounded with take", "[ranges][iota]") {
  auto result = std::views::iota(100, 200) | jal::ranges::take(5) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 100);
  REQUIRE(result[1] == 101);
  REQUIRE(result[2] == 102);
  REQUIRE(result[3] == 103);
  REQUIRE(result[4] == 104);
}

TEST_CASE("iota complex pipeline", "[ranges][iota]") {
  auto result = std::views::iota(1, 100) | jal::ranges::take(20) |
                jal::ranges::filter([](int x) { return x % 3 == 0; }) |
                jal::ranges::transform([](int x) { return x * 2; }) |
                jal::ranges::take(4) | jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 4);
  REQUIRE(result[0] == 6);
  REQUIRE(result[1] == 12);
  REQUIRE(result[2] == 18);
  REQUIRE(result[3] == 24);
}

TEST_CASE("iota with negative numbers", "[ranges][iota]") {
  auto result = std::views::iota(-5, 5) |
                jal::ranges::filter([](int x) { return x >= 0; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 0);
  REQUIRE(result[1] == 1);
  REQUIRE(result[2] == 2);
  REQUIRE(result[3] == 3);
  REQUIRE(result[4] == 4);
}

TEST_CASE("iota large numbers", "[ranges][iota]") {
  auto result = std::views::iota(1000, 1010) |
                jal::ranges::transform([](int x) { return x - 1000; }) |
                jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 10);
  for (int i = 0; i < 10; ++i) {
    REQUIRE(result[i] == i);
  }
}

TEST_CASE("iota fizzbuzz example", "[ranges][iota][example]") {
  auto result =
    std::views::iota(1, 31) | jal::ranges::take(15) |
    jal::ranges::filter([](int x) { return x % 3 == 0 || x % 5 == 0; }) |
    jal::ranges::to<std::vector<int>>();

  REQUIRE(result.size() == 7);
  REQUIRE(result[0] == 3);
  REQUIRE(result[1] == 5);
  REQUIRE(result[2] == 6);
  REQUIRE(result[3] == 9);
  REQUIRE(result[4] == 10);
  REQUIRE(result[5] == 12);
  REQUIRE(result[6] == 15);
}

TEST_CASE("to with std::deque", "[ranges][containers]") {
  std::vector<int> input{1, 2, 3, 4, 5};

  auto result = input | jal::ranges::transform([](int x) { return x * 2; }) |
                jal::ranges::to<std::deque<int>>();

  REQUIRE(result.size() == 5);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
  REQUIRE(result[3] == 8);
  REQUIRE(result[4] == 10);
}

TEST_CASE("to with std::list", "[ranges][containers]") {
  std::vector<int> input{5, 3, 8, 1, 9};

  auto result = input | jal::ranges::filter([](int x) { return x > 3; }) |
                jal::ranges::to<std::list<int>>();

  REQUIRE(result.size() == 3);
  auto it = result.begin();
  REQUIRE(*it++ == 5);
  REQUIRE(*it++ == 8);
  REQUIRE(*it++ == 9);
}

TEST_CASE("to with std::set removes duplicates", "[ranges][containers]") {
  std::vector<int> input{3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

  auto result = input | jal::ranges::to<std::set<int>>();

  REQUIRE(result.size() == 7);
  std::vector<int> expected{1, 2, 3, 4, 5, 6, 9};
  auto it = result.begin();
  for (int val : expected) {
    REQUIRE(*it++ == val);
  }
}

TEST_CASE("iota to different containers", "[ranges][iota][containers]") {
  auto iota_range = std::views::iota(10, 15);

  auto vec = iota_range | jal::ranges::to<std::vector<int>>();
  REQUIRE(vec.size() == 5);
  REQUIRE(vec[0] == 10);
  REQUIRE(vec[4] == 14);

  auto deq = iota_range | jal::ranges::to<std::deque<int>>();
  REQUIRE(deq.size() == 5);
  REQUIRE(deq.front() == 10);
  REQUIRE(deq.back() == 14);

  auto lst = iota_range | jal::ranges::to<std::list<int>>();
  REQUIRE(lst.size() == 5);
  REQUIRE(lst.front() == 10);
  REQUIRE(lst.back() == 14);
}

TEST_CASE("complex pipeline to list", "[ranges][containers]") {
  std::vector<int> input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = input | jal::ranges::filter([](int x) { return x % 2 == 1; }) |
                jal::ranges::transform([](int x) { return x * x; }) |
                jal::ranges::take(3) | jal::ranges::to<std::list<int>>();

  REQUIRE(result.size() == 3);
  auto it = result.begin();
  REQUIRE(*it++ == 1);
  REQUIRE(*it++ == 9);
  REQUIRE(*it++ == 25);
}

TEST_CASE("iota with deque", "[ranges][iota][containers]") {
  auto result = std::views::iota(100, 105) |
                jal::ranges::transform([](int x) { return x - 100; }) |
                jal::ranges::to<std::deque<int>>();

  REQUIRE(result.size() == 5);
  for (int i = 0; i < 5; ++i) {
    REQUIRE(result[i] == i);
  }
}

TEST_CASE("set maintains order", "[ranges][containers]") {
  std::vector<int> input{9, 2, 5, 1, 8, 3};

  auto result = input | jal::ranges::to<std::set<int>>();

  REQUIRE(result.size() == 6);
  int prev = -1;
  for (int val : result) {
    REQUIRE(val > prev);
    prev = val;
  }
}

TEST_CASE("multiset allows duplicates", "[ranges][containers]") {
  std::vector<int> input{1, 2, 2, 3, 3, 3, 4};

  auto result = input | jal::ranges::to<std::multiset<int>>();

  REQUIRE(result.size() == 7);
  REQUIRE(result.count(1) == 1);
  REQUIRE(result.count(2) == 2);
  REQUIRE(result.count(3) == 3);
  REQUIRE(result.count(4) == 1);
}
