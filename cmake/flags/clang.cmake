add_compile_options(
        -Wall -Wextra -Werror -Wpedantic
        -Wshadow -Wconversion -Wsign-conversion
        -Wold-style-cast -Wnon-virtual-dtor
        -Woverloaded-virtual -Wnull-dereference
        -Wdouble-promotion -Wformat=2
        -Wundef -Wextra-semi -Wimplicit-fallthrough
        -fno-common -fstrict-aliasing -fno-omit-frame-pointer
        -Wno-c++98-compat-extra-semi
)

if(NOT BUILD_TESTING)
    add_compile_options(
            -Wglobal-constructors
    )
endif ()