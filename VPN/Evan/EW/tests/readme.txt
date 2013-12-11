Test programs in this directory have names of the form test_xxx.
There is also a utility program filter_form3.

Use the -h option to get program usage.
They are intended to be run from a command line prompt in a console.

Some tests output timing information if executed with EW_DEBUG_PATTERN=timing.
Some have time consuming tests which are only performed if executed with
EW_DEBUG_PATTERN=long.
These will be indicated in usage message.

The exit code will be 0 for success, 1 for failure, which includes subtest
errors, errors in startup and shutdown code, uncaught exceptions and usage
errors.
A "test_xxx.ok" file will be created when a test is run completely and
successfully.
This will inhibit reruns of the test in a later "make test".
