# acatch
This is a fork of the  CATCH unit-test framework:
https://github.com/philsquared/Catch

The reporting and the logging part was mainly rewritten from scratch:
 - thread safe logging
 - assertions with minimal overhead: test is evaluated and no logging is performed on success
 - fixture vs. method tests
    - fixtures are created once and has a setup/teardown cycle
    - method tests instantiate new objects for each test-run
