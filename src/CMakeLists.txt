add_executable(proxy 
main.cc
types.cc
)

target_link_libraries(proxy sockpp)
target_link_libraries(proxy logpp)


target_include_directories(proxy PRIVATE ../includes)