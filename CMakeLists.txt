cmake_minimum_required(VERSION 2.6)
project(zolden)

add_executable(zold
	common/struct.cxx
	db/db.cxx
	db/dbhelper.cxx
	db/hashtable.cxx
	db/select.cxx
	db/subtables.cxx
	db/table.cxx
	lib/bio.cxx
	lib/file.cxx
	lib/misc.cxx
	lib/net.cxx
	lib/srxw.cxx
	server/client.cxx
	server/main.cxx
)

add_executable(zol
	common/struct.cxx
	client/main.cxx
	client/reader.cxx
	client/tokenizer.cxx
	client/query.cxx
	lib/bio.cxx
	lib/file.cxx
	lib/misc.cxx
	lib/net.cxx
)

target_include_directories(zold PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/lib
	${CMAKE_SOURCE_DIR}/server
)

target_include_directories(zol PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/client
	${CMAKE_SOURCE_DIR}/lib
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
		-std=c++11
	)
	target_compile_options(zol PUBLIC
		-std=c++11
	)
endif()

add_compile_options(
	-Wall
	-Werror
	-pedantic
)

target_compile_definitions(zold PUBLIC
	_POSIX_C_SOURCE=200809L
)

target_compile_definitions(zol PUBLIC
	_POSIX_C_SOURCE=200809L
)

target_link_libraries(zold PUBLIC
	pthread
)

install(TARGETS zold RUNTIME DESTINATION bin)
install(TARGETS zol RUNTIME DESTINATION bin)
