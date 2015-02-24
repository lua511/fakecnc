#!/bin/bash
# This will:
# exit 1 if the test failed to compile, or the workaround didnt work after all
# exit 0 if the test ran successfully
# if the LD_PRELOAD workaround is needed, echo the pathname of libstc++ on stdout

# here just return success and give a empty path
# the client doesn't load others because the empty path

echo ""
exit 0
