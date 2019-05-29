#!/bin/bash
#based on https://idiallo.com/blog/watching-folder-with-shell-script-bash
# colors 
if [ "$#" -ne 1 ] || ! [ -d "$1" ]; then
  echo "This script watch the dir for change"
  echo "Usage: $0 dir " >&2
  exit 1
fi

GREEN='\033[0;32m'
NC='\033[0m'

# run every interval in seconds
timeinterval=2;

# Main files and folders
SrcFolder=$1

echo "Folder=\"$SrcFolder\""

chksum1=`find $SrcFolder -type f -printf "%T@ %p\n" | md5sum | cut -d " " -f 1`;
while [[ true ]]; do
    chksum2=`find $SrcFolder -type f -printf "%T@ %p\n" | md5sum | cut -d " " -f 1`;
    if [[ $chksum1 != $chksum2 ]] ; then 
        printf "Changed!!\n";
#add your sync script here
        rsync -avz -e "ssh -p26793" --include '*.c'  .  cpc035@47.98.220.76:/home/export/base/CPC/cpc035/opanACC-test;
        printf "${GREEN}$(date) sync done. ${NC} \n";
#add your deploy script here
        printf "changes deployed \n"
        chksum1=$chksum2
    fi
    sleep $timeinterval;
done