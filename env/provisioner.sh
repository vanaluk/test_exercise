# Args
PROJECT_PATH=/home/vagrant/test_exercise/

export LANG=en_US.UTF-8
export LANGUAGE=en_US:en
export LC_ALL=en_US.UTF-8

export DEBIAN_FRONTEND=noninteractive
export WGET_ARGS="-q --show-progress --progress=bar:force:noscroll"
sudo apt update
sudo apt upgrade -y

# strictly necessary
sudo apt install -y build-essential

sudo apt -y install aptitude
sudo apt -y install libstdc++6
sudo apt -y install libc-dev
  
sudo apt update

sudo apt install -y make sudo curl unzip git \
    wget autoconf automake autotools-dev binutils binutils-common \
    binutils-x86-64-linux-gnu cmake cmake-data cpp distro-info-data file g++ \
    gcc libatomic1 libbinutils \
    libbrotli1 libc-dev-bin linux-libc-dev

# install mosquitto
sudo apt install -y mosquitto mosquitto-clients libmosquitto-dev
# install  dependents for libubox, uci, ubus and procd
sudo apt -y install lua5.1
sudo apt -y install liblua5.1-0-dev
sudo apt -y install libjson-c-dev
#sudo apt install libjsoncpp-dev
#sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json

cd /home/vagrant
mkdir third_party
cd third_party
# install libubox
git clone git://git.openwrt.org/project/libubox.git
cd libubox
mkdir build
cd build
cmake ..
make
sudo make install
# install ubus
cd /home/vagrant/third_party
git clone git://git.openwrt.org/project/ubus.git
cd ubus
mkdir build
cd build
cmake ..
make
sudo make install
# install uci
cd /home/vagrant/third_party
git clone git://git.openwrt.org/project/uci.git
cd uci
mkdir build
cd build
cmake ..
make
sudo make install
# install procd
cd /home/vagrant/third_party
git clone git://git.openwrt.org/project/procd.git
cd procd
mkdir build
cd build
cmake ..
make
sudo make install
# add lib to global path
sudo ldconfig /usr/local/lib

echo 'export PROJECT_PATH=/home/vagrant/test_exercise' >> /home/vagrant/.bashrc

# init test exercise config
sudo mkdir /etc/config
sudo touch /etc/config/wm2022
sudo uci set wm2022.wimark_test_exercise='wimark_config'
sudo uci commit  
  
# HACK: provisioner script can ONLY run as root, re-chown everything after provisioner finishes.
cd /home/vagrant
ls -A | grep -v keys | xargs sudo chown -R vagrant:vagrant
