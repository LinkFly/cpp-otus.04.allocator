cmake_minimum_required(VERSION 3.2)
project(allocator VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set(SOURCES
#	reserv_allocator.cpp
	main.cpp
	)
set(HEADERS
	share.h
	reserv_allocator.h
	helpers.h
	logger.h
	ptrs_manager.h
	direct_collector.h
	)
set(DATA_FILES
	

	)

add_executable(allocator ${SOURCES} ${HEADERS} ${DATA_FILES})
#add_library(reserv_allocator STATIC reserv_allocator.cpp reserv_allocator.h)
#target_link_libraries(allocator reserv_allocator)
add_executable(tests tests.cpp ${HEADERS})
set_target_properties(allocator tests PROPERTIES
#set_target_properties(allocator reserv_allocator PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

target_link_libraries(tests
	${Boost_LIBRARIES}
)

set_target_properties(tests PROPERTIES
	COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
	INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
	LINK_DIRECTORIES ${Boost_LIBRARY_DIRS} 
)
