add_executable(generate_fasta generate_fasta.cpp)
target_include_directories(generate_fasta PUBLIC ".")

add_executable(generate_pair generate_pair.cpp)
target_include_directories(generate_pair PUBLIC ".")