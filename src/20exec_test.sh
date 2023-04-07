#!/bin/bash
for i in {1..20}
do
	./tracer execute -u "ls"
done
./tracer execute -u "ls"
