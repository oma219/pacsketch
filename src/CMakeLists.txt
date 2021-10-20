add_executable(pacsketch pacsketch.cpp hash.cpp minhash.cpp)
target_link_libraries(pacsketch ${CMAKE_SOURCE_DIR}/zlib/libz.a)
target_include_directories(pacsketch PUBLIC "../include")

#add_executable(minhash minhash.cpp hash.cpp pacsketch.cpp)
#target_link_libraries(minhash ${CMAKE_SOURCE_DIR}/zlib/libz.a)
#target_include_directories(minhash PUBLIC "../include")

#add_executable(pacsketch hash.cpp)
#target_include_directories(pacsketch PUBLIC "../include")