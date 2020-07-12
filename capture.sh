# Restart
./spectre-meltdown-checker.sh

# Launch Malicious
source /opt/intel/sgxsdk/environment
./malicious/malicious

# LiME
time sudo insmod './lime.ko' "path=dump_lime_18_04_04.lime format=lime compress=0"; sha256sum dump_lime_18_04.lime

# AVML
time sudo ./avml dump_avml_18_04_04.lime; sha256sum dump_avml_18_04_4.lime

# Rekall
time sudo ./linpmem_2.0.1 -c zlib -o ./dump_rekall_18_04.aff4; sha256sum dump_rekall_18_04.aff4
time sudo ./linpmem_3.0.bin -c zlib -o ./dump_rekall_18_04_04.aff4; sha256sum dump_rekall_18_04.aff4

# Lime NG
cd lime_ng 
sudo ls
source /opt/intel/sgxsdk/environment
time ./lime_ng.sh dump_lime_18_04_04.raw

cat /proc/cpuinfo
cat /proc/meminfo