sudo su
apt update
apt install make
apt install g++
apt install cmake
apt install libmysqlclient-dev
apt install mysql-server
apt install --fix-missing
tar -zxvf mysql++-3.2.5.tar.gz

cd mysql++-3.2.5
./configure
make
make install
/sbin/ldconfig
cd ..

useradd -m judger
mkdir -m 755 /home/judger/client
cp -r ./data /home/judger
cp config /home/judger
chown judger:judger -R /home/judger
chmod 755 -R /home/judger/

cd judger_server
g++ judger_server.cpp -w -o judger_server -I/usr/local/include/mysql++ -lmysqlpp -I/usr/include/mysql -lmysqlclient
cp judger_server /usr/local/bin

cd ../judger_client
g++ judger_client.cpp -w -o judger_client -I/usr/local/include/mysql++ -lmysqlpp -I/usr/include/mysql -lmysqlclient
cp judger_client /usr/local/bin

/usr/local/bin/judger_server 1>/var/log/judger.log
