#!/bin/sh

[ -e temp/CombinedCabanaBoy.root ] && rm -f temp/CombinedCabanaBoy.root

root -l -b -q 'CombineCabanaBoy.C(0, 1, 0, 2)'
root -l -b -q 'CombineCabanaBoy.C(2, 3, 0, 2)'
root -l -b -q 'CombineCabanaBoy.C(4, 5, 0, 2)'
root -l -b -q 'CombineCabanaBoy.C(6, 9, 0, 2)'

echo "DONE"
