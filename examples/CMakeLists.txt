

add_executable(main main.cpp)
target_link_libraries(main PUBLIC event_sim)


add_executable(time_test time_test.cpp)
target_link_libraries(time_test PUBLIC event_sim)

