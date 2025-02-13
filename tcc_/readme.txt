1.编译tcc
tar -jxvf tcc-0.9.27.tar.bz2
./configure --disable-static  //static - no
make
make install

2.
gcc -o main main.c -ltcc -ldl

./main script.c
Hello, Alice!
Sum: 30
