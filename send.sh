#!/bin/bash

for i in `ls -1 *.h *.c`; do
	scp $i gilles@192.168.122.17:~/tcc/$i
done
