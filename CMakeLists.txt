cmake_minimum_required(VERSION 2.6)
project(database)

add_executable(database
	bitset.cxx
	db.cxx
	hashtable.cxx
	main.cxx
)

if($<VERSION_GREATER:CMAKE_VERSION,"3.0">)
	target_compile_features(database PUBLIC
		cxx_nullptr
	)
else()
	target_compile_options(database PUBLIC
		-std=c++11)
endif()

install(TARGETS database RUNTIME DESTINATION bin)
