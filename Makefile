#rely on CMake 3.0 or more
#rely on /etc/judger/config
#rely on /usr/bin/judger
#use mysql++3.2.5.tar.gz  -I/usr/local/include/mysql++   -lmysqlpp   ./configure    make    make install
#use libmysqlclient-dev   -I/usr/include/mysql   -lmysqlclient
#apt install libseccomp-dev libseccomp2 seccomp
#cp judger_client /usr/local/bin
#useradd -m judger && su judger && cd ~/
#mkdir data && mkdir judge


# apt update
# apt install cmake libmysqlclient-dev mysql-server libseccomp-dev libseccomp2 seccomp
# tar -zxvf mysql++3.2.5.tar.gz && cd mysql++3.2.5 && ./configure && make && make install
# useradd -m judger && su judger && cd ~/
# mkdir -m 744 data judge lib lib64 usr bin
# cd - && cp ./config ~/
# exit && chmod 700 config && chown judger:judger config

# -o  -g -I/usr/local/include/mysql++ -lmysqlpp -I/usr/include/mysql -lmysqlclient