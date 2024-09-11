find_dependency(Boost REQUIRED COMPONENTS container container_hash endian iterator range)
find_dependency(CGAL REQUIRED)
find_dependency(FastFloat REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/kigumiTargets.cmake")
