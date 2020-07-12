echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu xenial main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

sudo apt-get update -y
sudo apt-get install gcc git build-essential python-pip libssl-dev libcurl4-openssl-dev libprotobuf-dev -y
sudo apt-get install ocaml automake autoconf libtool wget python libssl-dev cmake perl -y

# Lime
git clone https://github.com/504ensicsLabs/LiME
cd LiME/src
make
cd ../..

# Rekall
sudo apt-get update -y
sudo apt-get install python-pip python-dev libssl-dev libncurses5-dev -y
sudo apt install virtualenv

pip install virtualenv
virtualenv /tmp/MyEnv
source /tmp/MyEnv/bin/activate
pip install --upgrade 'setuptools<45.0.0'
pip install --upgrade pip wheel
pip install pandas
pip install future==0.16.0
pip install rekall-agent rekall

# SGX
sudo apt-get install virtualbox-dkms
wget https://download.01.org/intel-sgx/sgx-linux/2.8/distro/ubuntu16.04-server/sgx_linux_x64_driver_2.6.0_51c4821.bin
chmod 755 sgx_linux_x64_driver_2.6.0_51c4821.bin
sudo ./sgx_linux_x64_driver_2.6.0_51c4821.bin

wget https://download.01.org/intel-sgx/sgx-linux/2.8/distro/ubuntu16.04-server/sgx_linux_x64_sdk_2.8.100.3.bin
chmod 755 sgx_linux_x64_sdk_2.8.100.3.bin
sudo ./sgx_linux_x64_sdk_2.8.100.3.bin

source /opt/intel/sgxsdk/environment

# Malicious 
cd malicious
make SGX_MODE=SIM
./malicious

# AVML
sudo ./avml --compress avml.lime

# Vulnerabilities Checker
wget https://meltdown.ovh -O spectre-meltdown-checker.sh
chmod +x spectre-meltdown-checker.sh
sudo ./spectre-meltdown-checker.sh

# SGX STEP
git clone https://github.com/jovanbulck/sgx-step.git
