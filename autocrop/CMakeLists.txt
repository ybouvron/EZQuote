cmake_minimum_required(VERSION 2.1)
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

project(autocrop  )

find_package( OpenCV REQUIRED )
add_executable( autocrop autocrop.cpp )
target_link_libraries( autocrop ${OpenCV_LIBS})
