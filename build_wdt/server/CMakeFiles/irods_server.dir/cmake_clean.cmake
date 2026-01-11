file(REMOVE_RECURSE
  ".5.0.90"
  "libirods_server.pdb"
  "libirods_server.so"
  "libirods_server.so.5.0.90"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/irods_server.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
