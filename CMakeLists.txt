cmake_minimum_required(VERSION 2.6)
project(database)

add_executable(database
	common/bitset.cxx
	common/hashtable.cxx
	db/db.cxx
	db/dbhelper.cxx
	db/dbtables.cxx
	db/select.cxx
	main.cxx
)

target_include_directories(database PUBLIC
	${CMAKE_SOURCE_DIR}/common
)

if($<VERSION_GREATER:CMAKE_VERSION,"3.0">)
	target_compile_features(database PUBLIC
		cxx_nullptr
	)
else()
	target_compile_options(database PUBLIC
		-std=c++11)
endif()

target_compile_options(database PUBLIC
	-Wall -Werror -pedantic)

install(TARGETS database RUNTIME DESTINATION bin)
