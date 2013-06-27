#!/bin/sh
# http://oderwat.de/2011/05/22/git-mit-revisions-nummern-ahnlich-wie-bei-subversion-nutzen/

# we start from SVN revision 1663
# the displayed version string reads 1.63
BASE_SVN_REV="1.63"


GIT_REV=`git rev-list HEAD | wc -l`

# has unnecessary whitespaces in string/output
#echo "GIT_REV=${GIT_REV}"

# http://linux.byexamples.com/archives/42/command-line-calculator-bc/
# scale means the precision of floating point, i.e. how many digit after the point
#echo "scale=3; ${BASE_SVN_REV} + ${GIT_REV}/1000.0 " | bc

# bc command to trim GIT_REV string
#GIT_REV_STR=`echo "${GIT_REV}" | bc`
#echo "${BASE_SVN_REV}.${GIT_REV_STR}"

# direct trim
echo "${BASE_SVN_REV}.${GIT_REV}" | tr -d ' '

# and some usage:
# mapping back to GIT IDs
# git show-rev-number 1234