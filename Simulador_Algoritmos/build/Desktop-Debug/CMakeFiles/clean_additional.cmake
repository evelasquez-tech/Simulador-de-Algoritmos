# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Simulador_Algoritmos_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Simulador_Algoritmos_autogen.dir/ParseCache.txt"
  "Simulador_Algoritmos_autogen"
  )
endif()
