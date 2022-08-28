add_executable(4vis
        ../common/bspfile.c
        ../common/cmdlib.c
        ../common/l3dslib.c
        ../common/llwolib.c
        ../common/mathlib.c
        ../common/mdfour.c
        ../common/scriplib.c
        ../common/threads.c
        ../common/trilib.c

        flow.c
        4vis.c
        )
target_link_libraries(4vis plcore)
