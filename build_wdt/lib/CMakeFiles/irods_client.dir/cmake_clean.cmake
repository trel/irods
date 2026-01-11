file(REMOVE_RECURSE
  ".5.0.90"
  "libirods_client.pdb"
  "libirods_client.so"
  "libirods_client.so.5.0.90"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/irods_client.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
