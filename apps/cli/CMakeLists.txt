add_executable(cli
    src/main.c
    src/options.c
)

target_link_libraries(cli PRIVATE core)

# Complier Warnings
if(MSVC)
    target_compile_options(cli PRIVATE /W4 /arch:AVX2)
else()
    target_compile_options(cli PRIVATE
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
