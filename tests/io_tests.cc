#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Region.h>
#include <kigumi/Region_io.h>

#include <sstream>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Region = kigumi::Region<K>;
using kigumi::io::read_obj;
using kigumi::io::read_off;
using kigumi::io::read_ply;
using kigumi::io::write_obj;
using kigumi::io::write_off;
using kigumi::io::write_ply;

TEST(IOTest, ObjEmpty) {
  std::stringstream ss;
  Region r;
  ASSERT_TRUE(write_obj(ss, Region::empty()));
  ASSERT_TRUE(read_obj(ss, r));
  ASSERT_TRUE(r.is_empty());
}

TEST(IOTest, ObjFull) {
  std::stringstream ss;
  Region r;
  ASSERT_TRUE(write_obj(ss, Region::full()));
  ASSERT_TRUE(read_obj(ss, r));
  ASSERT_TRUE(r.is_full());
}

TEST(IOTest, OffEmpty) {
  std::stringstream ss;
  Region r;
  ASSERT_TRUE(write_off(ss, Region::empty()));
  ASSERT_TRUE(read_off(ss, r));
  ASSERT_TRUE(r.is_empty());
}

TEST(IOTest, OffFull) {
  std::stringstream ss;
  Region r;
  ASSERT_TRUE(write_off(ss, Region::full()));
  ASSERT_TRUE(read_off(ss, r));
  ASSERT_TRUE(r.is_full());
}

TEST(IOTest, PlyEmpty) {
  std::stringstream ss;
  Region r;
  ASSERT_TRUE(write_ply(ss, Region::empty()));
  ASSERT_TRUE(read_ply(ss, r));
  ASSERT_TRUE(r.is_empty());
}

TEST(IOTest, PlyFull) {
  std::stringstream ss;
  Region r;
  ASSERT_TRUE(write_ply(ss, Region::full()));
  ASSERT_TRUE(read_ply(ss, r));
  ASSERT_TRUE(r.is_full());
}
