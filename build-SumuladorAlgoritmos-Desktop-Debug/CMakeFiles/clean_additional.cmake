# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/SumuladorAlgoritmos_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/SumuladorAlgoritmos_autogen.dir/ParseCache.txt"
  "SumuladorAlgoritmos_autogen"
  )
endif()
