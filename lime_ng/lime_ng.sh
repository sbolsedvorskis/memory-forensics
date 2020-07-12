sudo insmod './LiME/src/lime.ko' "path=./LiME/src/dump_lime.lime format=raw compress=0";
cd ./sgx
./sgx_dump
cd ..
cat ./LiME/src/dump_lime.lime ./sgx/sgx_dump.bin > dump_lime_ng.raw
