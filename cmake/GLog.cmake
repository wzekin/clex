add_library(glog INTERFACE)
target_include_directories(glog INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../external/plog/include)
