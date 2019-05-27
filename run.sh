#!/usr/bin/env zsh

LOG_DIR=/home/export/online1/cpc035/history_logs/log_`date "+%Y-%m-%d-%H:%M:%S"`
LOG_NOW=./log

RUN_MASK="${RUN_MASK:-1111}"
#ENABLE_7_16=1
#ENABLE_27_16=0
#ENABLE_7_64=0
#ENABLE_27_64=0

mkdir ${LOG_DIR}


echo "Generating latest executable file..."
make benchmark-optimized > ${LOG_DIR}/build.log 2>&1

if [[ `expr "${RUN_MASK} / 1000"` -eq "1" ]];then
    echo "Submitting 7_16..."
    bsub -b -o ${LOG_DIR}/7_16.log -q q_sw_cpc_1 -host_stack 1024 -share_size 4096 -n 16 -cgsp 64 \
        ./benchmark-optimized 7 512 512 512 48 \
        /home/export/online1/cpc/pre/stencil_data_7_512x512x512_48steps \
        /home/export/online1/cpc/pre/stencil_answer_7_512x512x512_48steps
fi

if [[ `expr "${RUN_MASK} / 100 % 10"` -eq "1" ]];then
    echo "Submitting 27_16..."
    bsub -b -o ${LOG_DIR}/27_16.log -q q_sw_cpc_1 -host_stack 1024 -share_size 4096 -n 16 -cgsp 64 \
        ./benchmark-optimized 27 512 512 512 16 \
        /home/export/online1/cpc/pre/stencil_data_27_512x512x512_16steps \
        /home/export/online1/cpc/pre/stencil_answer_27_512x512x512_16steps
fi

if [[ `expr "${RUN_MASK} / 10 % 10"` -eq "1" ]];then
    echo "Submitting 7_64..."
    bsub -b -o ${LOG_DIR}/7_64.log -q q_sw_cpc_1 -host_stack 1024 -share_size 4096 -n 64 -cgsp 64 \
        ./benchmark-optimized 7 768 768 768 64 \
        /home/export/online1/cpc/pre/stencil_data_7_768x768x768_64steps \
        /home/export/online1/cpc/pre/stencil_answer_7_768x768x768_64steps
fi

if [[ `expr "${RUN_MASK} % 10"` -eq "1" ]];then
    echo "Submitting 27_64..."
    bsub -b -o ${LOG_DIR}/27_64.log -q q_sw_cpc_1 -host_stack 1024 -share_size 4096 -n 64 -cgsp 64 \
        ./benchmark-optimized 27 768 768 768 16 \
        /home/export/online1/cpc/pre/stencil_data_27_768x768x768_16steps \
        /home/export/online1/cpc/pre/stencil_answer_27_768x768x768_16steps
fi

ln -s -f ${LOG_DIR} .
ln -s -f ${LOG_DIR} ${LOG_NOW}
