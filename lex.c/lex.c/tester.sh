#!/bin/bash

gcc lex.c
compiled=$?
if [[ $compiled != 0 ]]; then
	echo "does not compile"
	exit 1
fi

echo -n "Testing test1.txt : "
./a.out test1.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo "crashed"
	exit 1
fi
diff -w output.txt output1.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
	echo "incorrect output"
else
	echo "pass"
fi


echo -n "Testing test2.txt : "
./a.out test2.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo "crashed"
	exit 1
fi
diff -w output.txt output2.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
	echo "incorrect output"
else
	echo "pass"
fi

echo -n "Testing test3.txt : "
./a.out test3.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
    echo "crashed"
    exit 1
fi
diff -w output.txt output3.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
    echo "incorrect output"
else
    echo "pass"
fi

echo -n "Testing t1.txt : "
./a.out t1.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
    echo "crashed"
    exit 1
fi
diff -w output.txt o1.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
    echo "incorrect output"
else
    echo "pass"
fi

echo -n "Testing t2.txt : "
./a.out t2.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
    echo "crashed"
    exit 1
fi
diff -w output.txt o2.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
    echo "incorrect output"
else
    echo "pass"
fi

echo -n "Testing t3.txt : "
./a.out t3.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
    echo "crashed"
    exit 1
fi
diff -w output.txt o3.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
    echo "incorrect output"
else
    echo "pass"
fi

echo -n "Testing t4.txt : "
./a.out t4.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
    echo "crashed"
    exit 1
fi
diff -w output.txt o4.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
    echo "incorrect output"
else
    echo "pass"
fi

echo -n "Testing t5.txt : "
./a.out t5.txt > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
    echo "crashed"
    exit 1
fi
diff -w output.txt o5.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
    echo "incorrect output"
else
    echo "pass"
fi
