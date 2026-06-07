add_library(engine
    src/engine.c
    src/search.c
    src/static.c
)

target_include_directories(engine PUBLIC include)
target_link_libraries(engine PUBLIC core)

if(MSVC)
    target_compile_options(engine PRIVATE /W4 /arch:AVX2)
else()
    target_compile_options(engine PRIVATE
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
