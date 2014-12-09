#!/bin/bash
for filepath in /bin/*
do
	filename=$(basename $filepath)
	./tracelib "/bin/"$filename "-f" > ./test_result/$filename".txt"
done
	
