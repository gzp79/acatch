# acatch
This is fork of the  CATCH unit-test framework:
https://github.com/philsquared/Catch

The reporting and the logging part was mainly rewritten from scratch:
 - thread safe logging
 - assertions those have minimal overhead during success
    - only the test is evaluated and logging is performed on success
 - fixture vs. method tests
    - fixtures are created once and has a setup/teardown cycle
	- method test instantiate new objects for each tests
