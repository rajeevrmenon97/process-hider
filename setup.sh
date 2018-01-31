#!/bin/bash
read processCount
if ! [[ "$processCount" =~ ^[0-9]+$ ]]
    then
        echo "Sorry integers only"
fi
for (( i=1; i<=processCount; i++ ))
do
    read line
    processList=("${processList[@]}" $line)
done

echo ${processList[@]}

processArray="{"
for (( i=0; i<processCount; i++ ))
do
    processArray+="\"${processList[$i]}\","
done
processArray=${processArray::-1}"}"

echo "#define PROCESS_COUNT $processCount" > hidelib.c
echo "#define PROCESS_LIST $processArray" >> hidelib.c
tail -n +3 "proc_hide.c" >> "hidelib.c"
