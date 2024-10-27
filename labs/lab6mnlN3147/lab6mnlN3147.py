#!/usr/bin/python3

from re import compile

ipv6 = compile(r'(?<![:.\w])(?:[a-fA-F0-9]{1,4}:){7}[a-fA-F0-9]{1,4}(?![:.\w])')

class FormatError(Exception): pass

class UndoError(Exception): pass

class RedoError(Exception): pass

class MySet():
    def __init__(self, *args):
        self.set = self.form_set(*args)
        self.undo_stack = []
        self.redo_stack = []

    def form_set(self, *args):
        s = set()
        for i in args:
            if isinstance(i, str):
                if self.check_ipv6(i):
                    s.add(i)
            elif isinstance(i, (list, set, tuple)):
                for j in i:
                    if self.check_ipv6(j):
                        s.add(j)
            else:
                raise TypeError(f"{i} is not a valid type for an IPv6 address")
        return s

    def check_ipv6(self, element):
        if not isinstance(element, str):
            raise TypeError(element, "РІРІРµРґС‘РЅРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РЅРµ СЏРІР»СЏРµС‚СЃСЏ СЃС‚СЂРѕРєРѕР№!")
        if not ipv6.match(element):
            raise FormatError(f"РІРІРµРґС‘РЅРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ = '{element}' РЅРµ СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓРµС‚ С„РѕСЂРјР°С‚Сѓ IPv6!")
        return True

    def add(self, *args):
        s = self.form_set(*args)
        if s:
            self.undo_stack.append(self.set.copy())
            self.set.update(s)
    
    def remove(self, value):
        if value in self.set:
            self.undo_stack.append(self.set.copy())
            self.set.remove(value)
        else:
            raise ValueError
    
    def undo(self):
        if self.undo_stack:
            self.redo_stack.append(self.set.copy())
            self.set = self.undo_stack[-1].copy()
            self.undo_stack = self.undo_stack[:-1]
        else:
            raise UndoError("РёСЃС‚РѕСЂРёСЏ РјРЅРѕР¶РµСЃС‚РІР° РїСѓСЃС‚Р°!")

    def redo(self):
        if self.redo_stack:
            self.undo_stack.append(self.set.copy())
            self.set = self.redo_stack[-1]
            self.redo_stack = self.redo_stack[:-1]
        else:
            raise RedoError("РёСЃС‚РѕСЂРёСЏ РґР»СЏ redo РїСѓСЃС‚Р°!")
