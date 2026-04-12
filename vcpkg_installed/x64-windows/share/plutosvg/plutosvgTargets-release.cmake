#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "plutosvg::plutosvg" for configuration "Release"
set_property(TARGET plutosvg::plutosvg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(plutosvg::plutosvg PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/plutosvg.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plutosvg.dll"
  )

list(APPEND _cmake_import_check_targets plutosvg::plutosvg )
list(APPEND _cmake_import_check_files_for_plutosvg::plutosvg "${_IMPORT_PREFIX}/lib/plutosvg.lib" "${_IMPORT_PREFIX}/bin/plutosvg.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
