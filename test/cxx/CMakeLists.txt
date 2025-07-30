# compile command
add_executable(cond_var_version cond_var_version.cpp)
target_link_libraries(cond_var_version PRIVATE dice dice.h)
# target_compile_options(dice PRIVATE -fsanitize=address)
add_test(NAME cond_var_version COMMAND cond_var_version)
