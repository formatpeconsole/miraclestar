#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lunasvg::lunasvg" for configuration "Release"
set_property(TARGET lunasvg::lunasvg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lunasvg::lunasvg PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/lunasvg.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "plutovg::plutovg"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/lunasvg.dll"
  )

list(APPEND _cmake_import_check_targets lunasvg::lunasvg )
list(APPEND _cmake_import_check_files_for_lunasvg::lunasvg "${_IMPORT_PREFIX}/lib/lunasvg.lib" "${_IMPORT_PREFIX}/bin/lunasvg.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
