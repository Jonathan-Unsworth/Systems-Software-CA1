cd daemon
make clean
make
./myDaemon
sleep 1s
./myDaemon backup
sleep 5s
killall myDaemon
cat /var/log/syslog | grep myDaemon