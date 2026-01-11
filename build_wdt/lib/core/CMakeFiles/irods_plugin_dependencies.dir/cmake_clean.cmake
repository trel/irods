file(REMOVE_RECURSE
  ".5.0.90"
  "libirods_plugin_dependencies.pdb"
  "libirods_plugin_dependencies.so"
  "libirods_plugin_dependencies.so.5.0.90"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/irods_plugin_dependencies.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
