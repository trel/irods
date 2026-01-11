file(REMOVE_RECURSE
  "libRodsAPIs.a"
  "libRodsAPIs.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/RodsAPIs.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
