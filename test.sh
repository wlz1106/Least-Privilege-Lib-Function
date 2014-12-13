#!/bin/bash

ratio_result=test_result/ratio.txt
echo "Ratio Test"> $ratio_result
echo >> $ratio_result
for binfile in /bin/*; do
	filename=$(basename $binfile)
	result=test_result/$filename".txt"
	if [ "$1" = "-r" ]; then
		./tracelib $binfile -c >> $ratio_result
	elif [ "$1" = "-f" ]; then
		./tracelib $binfile -f > $result
	else
		./tracelib $binfile -c >> $ratio_result
		./tracelib $binfile -f > $result
	fi
	if [ $? -eq 0 ]; then
		echo "success:$binfile"
		continue
	else
		echo "fail:$binfile"
		continue
		echo "Enter return to continue"
	fi
done


