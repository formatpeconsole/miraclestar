#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lunasvg::lunasvg" for configuration "Debug"
set_property(TARGET lunasvg::lunasvg APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lunasvg::lunasvg PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/lunasvg.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "plutovg::plutovg"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/lunasvg.dll"
  )

list(APPEND _cmake_import_check_targets lunasvg::lunasvg )
list(APPEND _cmake_import_check_files_for_lunasvg::lunasvg "${_IMPORT_PREFIX}/debug/lib/lunasvg.lib" "${_IMPORT_PREFIX}/debug/bin/lunasvg.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
