#!/usr/bin/python3
import sys
import struct
from random import randint
from typing import List, AnyStr

def print_info():
    print("Р›СѓРєР°С€РµРЅРєРѕ РњР°СЂРёСЏ РќРёРєРѕР»Р°РµРІРЅР°, РіСЂ. N3147\nР’Р°СЂРёР°РЅС‚: 6-1")

def generate_string(*args) -> AnyStr:
    d = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`~!@\"в„–#;$%:^?&*()_-+=[{]}/,.<>|"

    if len(args) == 0:
        length = randint(10, 100)
    else:
        length = args[0]

    st = "\""
    for _ in range(length):
        ind = randint(0, len(d) - 1)
        if d[ind] == '\"':
            st += "\\"
        st += d[ind]
    st += "\""

    return st

def generate_list(N) -> List[AnyStr]:
    l = []
    for _ in range(N):
        l.append(generate_string())
    return l

def generate_indices(N) -> List[int]:
    l = []
    while len(l) < N:
        ind = randint(1, N)
        if ind not in l:
            l.append(ind)
    return l

def generate_random_data() -> AnyStr:
    return generate_string(randint(0, 10000))

def check_filename(filename):
    length = len(filename)
    if length < 4:
        return False
    if filename[-4:] != ".bin": 
        return False
    for i in range (0, length - 4):
        if (filename[i] not in "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890" or filename[i] == '.' or filename[i] == '_' or filename[i] == '-'):
            return False
    return True

def write_data_to_file(file_path, n):
    l = generate_list(n)
    indices = generate_indices(n)

    with open(file_path, 'wb') as f:
        f.write(struct.pack('<I', n))

        for s in l:
            for char in s:
                byte = char.encode('utf-8')
                f.write(byte)
            f.write(b'\x00')

        f.write((generate_random_data().encode('utf-8') + b'\x00'))

        for ind in indices:
            f.write(struct.pack('<H', ind))
    
    print(f"РЎРіРµРЅРµСЂРёСЂРѕРІР°РЅ С„Р°Р№Р» '{file_path}', С‡РёСЃР»Рѕ СЃС‚СЂРѕРє РІ РєРѕС‚РѕСЂРѕРј = {n}:")
    for i in range(n):
        print(f"СЃС‚СЂРѕРєР°: {l[i]}, РёРЅРґРµРєСЃ: {indices[i]}")

def main():
    n = randint(10, 1000)
    file_path = None
    
    for i in range(1, len(sys.argv)):
        if sys.argv[i] == '-v':
            print_info()
            sys.exit(0)
        elif sys.argv[i] == '-n':
            if i + 1 < len(sys.argv):
                try:
                    n = int(sys.argv[i + 1])
                except ValueError:
                    print("РћС€РёР±РєР°: РќРµРєРѕСЂСЂРµРєС‚РЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РґР»СЏ РѕРїС†РёРё -n.", file=sys.stderr)
                    sys.exit(1)
            else:
                print("РћС‚СЃСѓС‚СЃС‚РІСѓРµС‚ Р·РЅР°С‡РµРЅРёРµ РїРѕСЃР»Рµ РѕРїС†РёРё -n.", file=sys.stderr)
                sys.exit(1)
        elif sys.argv[i][0] == "-":
            print(f"Р’РІРµРґРµРЅР° РЅРµРІРµСЂРЅР°СЏ РѕРїС†РёСЏ {sys.argv[i]}.", file=sys.stderr)
            sys.exit(1)
        elif check_filename(sys.argv[i]):
            file_path = sys.argv[i]
    
    if file_path == None:
        print("РћС€РёР±РєР°: РЅРµ СѓРєР°Р·Р°РЅ РїСѓС‚СЊ Рє С„Р°Р№Р»Сѓ РёР»Рё СѓРєР°Р·Р°РЅ РЅРµРІРµСЂРЅС‹Р№ С„РѕСЂРјР°С‚: РѕР¶РёРґР°Р»СЃСЏ Р±РёРЅР°СЂРЅС‹Р№ С„РѕСЂРјР°С‚ (.bin).", file=sys.stderr)
        sys.exit(1)
    

    write_data_to_file(file_path, n)
    
if __name__ == "__main__":
    main()
