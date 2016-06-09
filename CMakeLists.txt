### Generic native Skirmish AI config
#

set(mySourceDirRel         "src/") # Common values are "" or "src"
set(additionalSources      ${CMAKE_SOURCE_DIR}/rts/System/Util.cpp)
set(additionalLibraries    ${Cpp_AIWRAPPER_TARGET} CUtils)

# try std::regex, use boost::regex otherwise
include(CMakePushCheckState)
include(CheckCXXSourceRuns)
cmake_push_check_state()
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
check_cxx_source_runs("
  #include <regex>
  int main() {
    return std::regex_match(\"StackOverflow\", std::regex(\"(stack)(.*)\", std::regex::ECMAScript | std::regex::icase)) ? 0 : 1;
  }
" HAVE_CXX_11_REGEX)
cmake_pop_check_state()
if    (HAVE_CXX_11_REGEX)
	add_definitions(-DHAVE_CXX_11_REGEX)
else  (HAVE_CXX_11_REGEX)
	set(Boost_USE_STATIC_LIBS   ON)
	find_package(Boost REQUIRED regex thread)
	set(additionalLibraries    ${additionalLibraries} boost_regex boost_thread)
endif (HAVE_CXX_11_REGEX)

#set(additionalCompileFlags "-Isrc/lib/ -Isrc/circuit/ -O1 -DDEBUG -DDEBUG_VIS -DDEBUG_LOG -Wall -Wextra -D_GLIBCXX_USE_CXX11_ABI=0")
# Spring 100.0 was compiled with old ABI
if ("${CMAKE_SYSTEM}" MATCHES "ARCH")
	set(additionalCompileFlags "-DDEBUG_VIS -Wall -D_GLIBCXX_USE_CXX11_ABI=1")
else ()
	set(additionalCompileFlags "-Wall -D_GLIBCXX_USE_CXX11_ABI=0")
endif ()

set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -static-libgcc -static-libstdc++")


if    (BUILD_Cpp_AIWRAPPER)
	include_directories(BEFORE
		${Cpp_AIWRAPPER_INCLUDE_DIRS}
		${SDL2_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/src/lib/
		${CMAKE_CURRENT_SOURCE_DIR}/src/circuit/
	)
	ConfigureNativeSkirmishAI(mySourceDirRel additionalSources additionalCompileFlags additionalLibraries)
else  (BUILD_Cpp_AIWRAPPER)
	message ("warning: (New) C++ Circuit AI will not be built! (missing Cpp Wrapper)")
endif (BUILD_Cpp_AIWRAPPER)
