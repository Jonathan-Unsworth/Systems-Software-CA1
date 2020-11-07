cd daemon
make clean
make
./myDaemon 
sleep 1s
./myDaemon ${1}
sleep 5s
killall myDaemon
cat /var/log/syslog | grep myDaemon