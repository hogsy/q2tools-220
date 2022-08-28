add_executable(4bsp
        ../common/bspfile.c
        ../common/cmdlib.c
        ../common/mathlib.c
        ../common/polylib.c
        ../common/scriplib.c
        ../common/threads.c

        brushbsp.c
        csg.c
        faces.c
        leakfile.c
        map.c
        portals.c
        prtfile.c
        4bsp.c
        textures.c
        tree.c
        writebsp.c
        )
target_link_libraries(4bsp plcore)
