cmake_minimum_required(VERSION 2.6)
project(zolden)

add_executable(zold
	client/query.cxx
	common/bitset.cxx
	common/hashtable.cxx
	common/misc.cxx
	db/db.cxx
	db/dbhelper.cxx
	db/dbtables.cxx
	db/select.cxx
	server/main.cxx
)

add_executable(zol
	client/main.cxx
	client/reader.cxx
	client/tokenizer.cxx
	common/misc.cxx
	db/dbhelper.cxx
	db/select.cxx
)

target_include_directories(zold PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/server
)

target_include_directories(zol PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/client
)

if($<VERSION_GREATER:CMAKE_VERSION,"3.0">)
	target_compile_features(zold PUBLIC
		cxx_nullptr
		cxx_rvalue_reference
	)
	target_compile_features(zol PUBLIC
		cxx_autotype
		cxx_nullptr
		cxx_rvalue_reference
	)
else()
	target_compile_options(zold PUBLIC
		-std=c++11)
	target_compile_options(zol PUBLIC
		-std=c++11)
endif()

target_compile_options(zold PUBLIC
	-Wall -Werror -pedantic)

target_compile_options(zol PUBLIC
	-Wall -Werror -pedantic)

install(TARGETS zold RUNTIME DESTINATION bin)
install(TARGETS zol RUNTIME DESTINATION bin)
