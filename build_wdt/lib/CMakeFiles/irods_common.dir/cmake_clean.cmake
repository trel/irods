file(REMOVE_RECURSE
  ".5.0.90"
  "libirods_common.pdb"
  "libirods_common.so"
  "libirods_common.so.5.0.90"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/irods_common.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
