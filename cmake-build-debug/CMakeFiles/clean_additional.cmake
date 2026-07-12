# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ProteusClone_OOP_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ProteusClone_OOP_autogen.dir\\ParseCache.txt"
  "ProteusClone_OOP_autogen"
  )
endif()
