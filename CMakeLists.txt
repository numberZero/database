cmake_minimum_required(VERSION 2.6)
project(database)

add_executable(database
	bitset.cxx
	main.cxx
)

target_compile_features(database PUBLIC
	cxx_nullptr
)

install(TARGETS database RUNTIME DESTINATION bin)
