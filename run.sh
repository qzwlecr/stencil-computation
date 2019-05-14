#前台提交
bsub -b -I -q q_sw_expr -host_stack 1024 -share_size 4096 -n 16 -cgsp 64 ./benchmark-naive 7 512 512 512 48 /home/export/online1/cpc/pre/stencil_data_512x512x512 /home/export/online1/cpc/pre/stencil_answer_7_512x512x512_48steps 2>&1 | tee run.log
#后台提交
#bsub -b -o run.loh -q q_sw_expr -host_stack 1024 -share_size 4096 -n 16 -cgsp 64 ./benchmark-nativ 7 512 512 512 48 /home/export/online1/cpc/pre/stencil_data_512x512x512 /home/export/online1/cpc/pre/stencil_answer_7_512x512x512_48steps
