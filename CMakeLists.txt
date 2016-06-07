
set( acatch_src_public
  "acatch/acatch_expressioncapture.hpp"
  "acatch/acatch_fatalcondition.hpp"
  "acatch/acatch_framework.hpp"
  "acatch/acatch.hpp"
  "acatch/acatch_core.hpp"
  "acatch/acatch_registry.hpp"
  "acatch/acatch_section.hpp"
  "acatch/acatch_simpletestreport.hpp"
  "acatch/acatch_string.hpp"
  "acatch/acatch_testassert.hpp"
  "acatch/acatch_testcaseresult.hpp"
  "acatch/acatch_testcasetracker.hpp"
  "acatch/acatch_testreport.hpp"
  "acatch/acatch_tostring.hpp"
)

set( acatch_incdir_public
  ${acatch_config_include_dir}
  "."
)

set( acatch_src_private
  "acatch/test/test_exceptiontests.ipp"
  "acatch/test/test_parttracker.ipp"
  "acatch/test/test_tostringpair.ipp"
  "acatch/test/test_tostringtuple.ipp"
  "acatch/test/test_tostringvector.ipp"
  "acatch/test/test_tostringwhich.ipp"

  "src/acatch_fatalcondition.cpp"
  "src/acatch_framework.cpp"
  "src/acatch_registry.cpp"
  "src/acatch_section.cpp"
  "src/acatch_simpletestreport.cpp"
  "src/acatch_tostring.cpp"
)

add_library( "acatch" STATIC ${acatch_src_public} ${acatch_src_private} )
target_include_directories( "acatch" PUBLIC ${acatch_incdir_public} )

