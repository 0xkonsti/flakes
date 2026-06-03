add_library(core
    src/board.c
    src/fen.c
    src/zobrist.c
    src/move_lut.c
    src/legal.c
    src/algebraic.c
)

if(MSVC)
    target_compile_options(core PRIVATE /W4 /arch:AVX2)
else()
    target_compile_options(core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wformat=2
        -Wundef
        -Wcast-qual
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wwrite-strings
        -Wnull-dereference
        -Wimplicit-fallthrough
        -Wmisleading-indentation
        -mbmi2
    )
endif()

target_include_directories(core PUBLIC include)

target_link_libraries(core PUBLIC utils)
