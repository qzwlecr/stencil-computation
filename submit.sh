#!/bin/zsh

rm -rf submit.log

for i in `seq 1 4`
do
expect -c "
    set timeout -1
    log_file submit.log
    spawn ./cpcrun
    expect \"Please enter the queue and the number of the example:\"
    send \"q_sw_cpc_1 $i\r\"
    expect \"Do you want to update your record?(Y/N):\"
    send \"Y\r\"
    expect eof"
done
