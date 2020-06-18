
MODULES="dht22.cpp threads.cpp"

for MODULE in $MODULES
do
  g++ -c $MODULE
  [ $? != 0 ] && exit 2
done


g++ -lpthread -lwiringPi *.o main.cpp -o dht22
[ $? != 0 ] && exit 2
