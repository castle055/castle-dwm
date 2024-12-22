#!/bin/bash

cd /home/castle/corium/sources/castle-dwm/graphs

rm ./import_graph.gv

echo "Looking for files and generating graph data file"

echo "digraph Imports {" >> ./import_graph.gv

find ../src/ -name '*.cppm' | xargs rg -oNI "^export module (.*);|import (.*);" | sed -r 's/export module (.*);/\t}\n\t\1 -> {/' | sed -r 's/import /\t\t/' | sed 's/\./__/g;s/:/_P_/g' | tail -n+2 >> ./import_graph.gv

echo "}}" >> ./import_graph.gv

echo "Generating graph..."
dot -Tpng import_graph.gv -o import_graph_dot.png
#echo "Generating graph..."
#neato -Tpng import_graph.gv -o import_graph_neato.png
#echo "Generating graph..."
#twopi -Tpng import_graph.gv -o import_graph_twopi.png
#echo "Generating graph..."
#circo -Tpng import_graph.gv -o import_graph_circo.png
#echo "Generating graph..."
#fdp -Tpng import_graph.gv -o import_graph_fdp.png
#echo "Generating graph..."
#sfdp -Tpng import_graph.gv -o import_graph_sfdp.png
#echo "Generating graph..."
#patchwork -Tpng import_graph.gv -o import_graph_patchwork.png
#echo "Generating graph..."
#osage -Tpng import_graph.gv -o import_graph_osage.png
echo "All graphs done."

feh import_graph_dot.png
