cmake_minimum_required(VERSION 3.1...3.14)

# Back compatibility for VERSION range
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(gigamonkey 	VERSION 1.0
		DESCRIPTION "open-source Bitcoin library in c++"
		LANGUAGES CXX)

# Set cmake as import path for Find*.cmake files
#set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Setup paths to outputs of conan's "cmake_find_package" generator
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
## Check if GTests is installed. If not, install it

option(PACKAGE_TESTS "Build the tests" ON)
if(NOT TARGET gtest_main AND PACKAGE_TESTS)
	# Download and unpack googletest at configure time
	configure_file(cmake/gtests.txt.in googletest-download/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
	if(result)
		message(FATAL_ERROR "CMake step for googletest failed: ${result}")
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
	if(result)
		message(FATAL_ERROR "Build step for googletest failed: ${result}")
	endif()

	# Prevent overriding the parent project's compiler/linker
	# settings on Windows
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	
	# Add googletest directly to our build. This defines
	# the gtest and gtest_main targets.
	add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
			${CMAKE_CURRENT_BINARY_DIR}/googletest-build)
include(GoogleTest)

endif()

    add_definitions("-DHAS_BOOST")
include(CTest)

if(PACKAGE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

add_library(gigamonkey STATIC 
    src/sv/utilstrencodings.cpp
    src/sv/random.cpp
    src/sv/big_int.cpp
    src/sv/script/opcodes.cpp
    src/sv/script/script_error.cpp
    src/sv/script/script_num.cpp
    src/sv/arith_uint256.cpp
    src/sv/pubkey.cpp
    src/sv/primitives/transaction.cpp
    src/sv/script/limitedstack.cpp
    src/sv/script/script.cpp
    src/sv/config.cpp
    src/sv/script/interpreter.cpp
    src/sv/policy/policy.cpp
    
    src/bitcoin_sv/hash.cpp
    src/bitcoin_sv/signature.cpp
    src/bitcoin_sv/script.cpp
    #src/bitcoin_sv/sv.cpp 
    src/gigamonkey/timestamp.cpp
    src/gigamonkey/secp256k1.cpp
    src/gigamonkey/script.cpp
    src/gigamonkey/address.cpp
    src/gigamonkey/wif.cpp
    src/gigamonkey/merkle.cpp
    src/gigamonkey/timechain.cpp
    src/gigamonkey/work.cpp
    src/gigamonkey/redeem.cpp
    src/gigamonkey/schema/hd.cpp
    src/gigamonkey/schema/random.cpp
    src/gigamonkey/wallet.cpp
    src/gigamonkey/spv.cpp
    src/gigamonkey/accounts.cpp
    src/gigamonkey/merkle/dual.cpp
    src/gigamonkey/stratum/method.cpp
    src/gigamonkey/stratum/error.cpp
    src/gigamonkey/stratum/stratum.cpp
    src/gigamonkey/stratum/mining_subscribe.cpp
    src/gigamonkey/stratum/mining_authorize.cpp
    src/gigamonkey/stratum/mining.cpp
    src/gigamonkey/stratum/extensions.cpp
    src/gigamonkey/boost/boost.cpp
    src/gigamonkey/mapi/mapi.cpp
    src/gigamonkey/mapi/envelope.cpp
)

target_link_libraries(gigamonkey PUBLIC CONAN_PKG::data util CONAN_PKG::boost CONAN_PKG::openssl CONAN_PKG::cryptopp CONAN_PKG::nlohmann_json CONAN_PKG::gmp )

target_include_directories(gigamonkey PUBLIC include)

# Set C++ version
target_compile_features(gigamonkey PUBLIC cxx_std_17)
set_target_properties(gigamonkey PROPERTIES CXX_EXTENSIONS OFF)
