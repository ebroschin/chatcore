add_compile_options(
    -Wall -Wextra -Werror -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -Wold-style-cast -Wnon-virtual-dtor
    -Woverloaded-virtual -Wnull-dereference
    -Wdouble-promotion -Wformat=2
    -Wundef -Wextra-semi -Wimplicit-fallthrough
    -fno-common -fstrict-aliasing -fno-omit-frame-pointer
    -Wno-error=null-dereference
    -Wno-error=tsan
)