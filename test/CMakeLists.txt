add_executable(stream_test serializer.cpp main.cpp)
target_link_libraries(stream_test PUBLIC stream)