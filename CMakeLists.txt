cmake_minimum_required(VERSION 2.6)
project(zolden)

option(USE_EXCLUSIVE_LOCK "Disallow requests to be processed simultaneously" OFF)
option(USE_MANUAL_TCP_FLUSH "Flush TCP stream manually instead of system default settings" OFF)
option(DISCONNECT_ON_INVALID_REQUEST "Disconnect anyone who have sent an incorrect request" OFF)

add_executable(zold
	common/struct.cxx
	db/db.cxx
	db/dbhelper.cxx
	db/hashers.cxx
	db/hashtable.cxx
	db/select.cxx
	db/subdb_string.cxx
	db/subdb_struct.cxx
	db/subtable.cxx
	db/table.cxx
	lib/args.cxx
	lib/bio.cxx
	lib/fs.cxx
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
	lib/args.cxx
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

if(USE_EXCLUSIVE_LOCK)
	target_compile_definitions(zold PRIVATE USE_EXCLUSIVE_LOCK)
endif()

if(USE_MANUAL_TCP_FLUSH)
	target_compile_definitions(zol PRIVATE USE_MANUAL_TCP_FLUSH)
	target_compile_definitions(zold PRIVATE USE_MANUAL_TCP_FLUSH)
endif()

if(DISCONNECT_ON_INVALID_REQUEST)
	target_compile_definitions(zold PRIVATE DISCONNECT_ON_INVALID_REQUEST)
endif()

target_link_libraries(zol PUBLIC
	pthread
)

target_link_libraries(zold PUBLIC
	pthread
)

install(TARGETS zold RUNTIME DESTINATION bin)
install(TARGETS zol RUNTIME DESTINATION bin)
