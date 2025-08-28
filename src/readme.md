hello world

g++ -O2 -std=c++17 -o semicolons   src/main.cpp   src/compressorController.cpp   src/compressionContext.cpp   src/utils.cpp   src/compressor.cpp

./semicolons < tests/input.txt > tests/output.csv
./semicolons < tests/input2.txt > tests/output2.csv

python3 tests/bench_speed.py ./semicolons tests/input.txt tests/input2.txt
python eval_compression.py --input tests/input.txt --output tests/output.csv
