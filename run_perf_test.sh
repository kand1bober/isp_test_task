g++ src/main.cpp -o $1

cp $1 performance/$1

cd performance

sudo perf record -a -F 99 -g ./$1 -bench

sudo perf script > perf.script

~/FlameGraph/stackcollapse-perf.pl perf.script | ~/FlameGraph/flamegraph.pl > flamegraph.svg

rm $1