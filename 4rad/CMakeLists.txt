add_executable(4rad
        ../common/bspfile.c
        ../common/cmdlib.c
        ../common/lbmlib.c
        ../common/mathlib.c
        ../common/polylib.c
        ../common/scriplib.c
        ../common/threads.c

        lightmap.c
        patches.c
        4rad.c
        trace.c
        )
target_link_libraries(4rad plcore)
