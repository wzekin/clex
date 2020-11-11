set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(HatTrie)
include(GLog)
include(Warnings)
set(SOURCES          # All .cpp files in src/
   ${CMAKE_CURRENT_LIST_DIR}/src/reader.cpp
   ${CMAKE_CURRENT_LIST_DIR}/src/type.cpp
   ${CMAKE_CURRENT_LIST_DIR}/src/lex.cpp
)
set(LIBRARY_NAME clex)  # Default name for the library built from src/*.cpp (change if you wish)

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library.
add_library(${LIBRARY_NAME} OBJECT ${SOURCES})

# Lib needs its header files, and users of the library must also see these (PUBLIC). (No change needed)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# There's also (probably) doctests within the library, so we need to see this as well.
target_link_libraries(${LIBRARY_NAME} PUBLIC hat-trie)
target_link_libraries(${LIBRARY_NAME} PUBLIC glog)

# Set the compile options you want (change as needed).
target_set_warnings(${LIBRARY_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
# target_compile_options(${LIBRARY_NAME} ... )  # For setting manually.
