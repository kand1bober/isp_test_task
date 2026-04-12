g++ src/main.cpp -o $1

cp $1 performance/$1

cd performance

sudo perf record -a -F 99 -g ./a 

sudo perf script > perf.script

# ./../../FlameGraph/stackcollapse-perf.pl out.perf > out.folded

# ./../../FlameGraph/flamegraph.pl out.perf > flamegraph.svg

~/FlameGraph/stackcollapse-perf.pl perf.script | ~/FlameGraph/flamegraph.pl > flamegraph.svg
