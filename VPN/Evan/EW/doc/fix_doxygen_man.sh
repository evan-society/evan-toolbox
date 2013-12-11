#!/bin/sh

# Usage: $0 <name> <version> <file> ...

# This polishes the doxygen man pages a little.
# It will probably break if the doxygen man page output format changes.

# Changes:
#   Convert tex R^2 and R^3 to ascii.
#   Strip the "Generated automatically by ..." Author section.
#   Convert
#     .TH "ew::Transform2" 3 "5 Apr 2009" "Version 4.0" "EW Library" \" ...
#   to
#     .TH "ew::Transform2" 3 "$version" "$name" "$name"
#   etc.
#   Rewrite the section headers.
#   Move the brief description to the NAME section.
#   Move the member list to SYNOPSIS section.

set -e
name=$1
ver=$2
shift
shift

for i in $* ; do
  awk -v name="$name" -v ver="$ver" '
    {gsub(/\$\\mathbb{R}\^2\$/, "\\fBR\\fP^2")}
    {gsub(/\$\\mathbb{R}\^3\$/, "\\fBR\\fP^3")}
    /^\.TH/ {$0 = $1 " " $2 " " $3 " \"" ver "\" \"" name "\" \"" name "\""}
    /^\.SH "Detailed Description"/ {$0 = ".SH DESCRIPTION"}
    /^\.SH "Constructor & Destructor Documentation"/ \
     {$0 = ".SH CONSTRUCTORS AND DESCTRUCTORS"}
    /^\.SH "Member Data Documentation"/ {$0 = ".SH MEMBER DATA"}
    /^\.SH "Member Enumeration Documentation"/ {$0 = ".SH MEMBER ENUMERATIONS"}
    /^\.SH "Member Function Documentation"/ {$0 = ".SH MEMBER FUNCTIONS"}
    /^\.SH "Member Typedef Documentation"/ {$0 = ".SH MEMBER TYPEDEFS"}
    /^\.SH SYNOPSIS/ {insyn = 1; syn = $0 "\n"; next}
    /^\.SH DESCRIPTION/ {indesc = 1; dl = 1; desc = $0 "\n"; insyn = 0; next}
    /^\.SS/ && indesc == 1 {indesc2 = 1}
    /^\.SH/ && indesc == 1 {printf "%s", syn desc; indesc = 0}
    insyn == 1 {syn = syn $0 "\n"; next}
    indesc == 1 && dl == 2 {print; dl++; next}
    indesc == 1 && indesc2 == 1 {dl++; syn = syn $0 "\n"; next}
    indesc == 1 {dl++; desc = desc $0 "\n"; next}
    /^\.SH "Author"/ {exit}
    {print}
  ' $i > $i.tmp
  mv $i.tmp $i
done

exit 0
