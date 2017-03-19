#!/bin/sh

# Give this some color!
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

# Remove the previous performance data to avoid confusion
rm perf.txt 2>/dev/null 1>&2

# Compile entire project
echo "Compiling..."
make
echo "==========================================="
echo "         Finished compiling!"
echo "==========================================="
echo "     All performance output can be"
echo "         found in perf.txt"
echo "==========================================="

# Generate 2048 key for duration of system test
echo "Creating RSA key..."
openssl genrsa -des3 -out private.pem -passout pass:foobar 2048 2>/dev/null 1>&2

# Create a 1GB random file
# Also satisfies the following condition:
# file where the length % 8 (word size) = 0
echo "Creating 1GB random file..."
dd if=/dev/urandom of=randomfile1GB bs=1024k count=1000 2>/dev/null 1>&2

# Create a 1GB zero file
# Also satisfies the following condition:
# file where the length % 8 (word size) = 0
echo "Creating 1GB zero fie..."
dd if=/dev/zero of=zerofile1GB bs=1024k count=1000 2>/dev/null 1>&2

# Create an empty file
echo "Creating empty file..."
touch empty.txt 2>/dev/null 1>&2

# Create a file where the length % 8 (word size) != 0
# and the length > 8
echo "Creating over file..."
echo "Hello World!" > over.txt 

# Create a file where the length % 8 (word size) != 0
# and the length < 8
echo "Creating under file..."
echo "Hello!" > under.txt 

# Test each file and perform a binary diff to
# ensure the plain text and decrypted files match
echo "Testing 1GB random file..."
echo "=========================================================" >> perf.txt
echo "Testing 1GB random file..." >> perf.txt
./main randomfile1GB >> perf.txt
if ! diff -q randomfile1GB decrypted_file.txt > /dev/null  2>&1; then
  echo "	${RED}1GB random file failed!${NC}"
else
  echo "	${GREEN}1GB random file passed!${NC}"
fi
echo "=========================================================" >> perf.txt

echo "Testing zero file..."
echo "=========================================================" >> perf.txt
echo "Testing zero file..." >> perf.txt
./main zerofile1GB >> perf.txt
if ! diff -q zerofile1GB decrypted_file.txt > /dev/null  2>&1; then
  echo "	${RED}1GB zero file failed!${NC}"
else
  echo "	${GREEN}1GB zero file passed!${NC}"
fi
echo "=========================================================" >> perf.txt

echo "Testing empty file..."
echo "=========================================================" >> perf.txt
echo "Testing empty file..." >> perf.txt
./main empty.txt >> perf.txt
if ! diff -q empty.txt decrypted_file.txt > /dev/null  2>&1; then
  echo "	${RED}empty file failed!${NC}"
else
  echo "	${GREEN}empty file passed!${NC}"
fi
echo "=========================================================" >> perf.txt

echo "Testing over file..."
echo "=========================================================" >> perf.txt
echo "Testing over file..." >> perf.txt
./main over.txt >> perf.txt
if ! diff -q over.txt decrypted_file.txt > /dev/null  2>&1; then
  echo "	${RED}over file failed!${NC}"
else
  echo "	${GREEN}over file passed!${NC}"
fi
echo "=========================================================" >> perf.txt

echo "Testing under file..."
echo "=========================================================" >> perf.txt
echo "Testing under file..." >> perf.txt
./main under.txt >> perf.txt
if ! diff -q under.txt decrypted_file.txt > /dev/null  2>&1; then
  echo "	${RED}under file failed!${NC}"
else
  echo "	${GREEN}under file passed!${NC}"
fi
echo "=========================================================" >> perf.txt

# Cleanup
echo "Cleaning up..."
rm empty.txt randomfile1GB zerofile1GB private.pem over.txt under.txt
make clean

echo "Done!"
