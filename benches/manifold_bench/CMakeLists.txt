# Installing manifold:
#
# ```
# git clone --recurse-submodules https://github.com/elalish/manifold.git
# cd manifold
# mkdir build && cd build
# cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install -DMANIFOLD_EXPORT=ON -DMANIFOLD_PAR=TBB
# ninja install
# ```
#
# Consuming manifold in kigumi:
#
# ```
# CMAKE_PREFIX_PATH=../manifold/install ./run c
# ```

set(TARGET manifold_bench)

add_executable(${TARGET}
    main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    manifold
    meshIO
)
