import pytest
from lab6mnlN3147 import FormatError, UndoError, RedoError, MySet
from tempfile import TemporaryFile

def test_make_a_set():
    s1 = MySet()
    assert s1.set == set()

    ip6 = ["1050:0000:0000:0000:0005:0600:300c:326b", "FF80:0000:0000:0000:0123:1234:ABCD:EF12"]
    s2 = MySet(ip6)
    assert s2.set == set(ip6)

def test_format():
    with pytest.raises(FormatError):
        s1 = MySet("127.0.0.1")
        assert s1.set == set() # РІ СЃР»СѓС‡Р°Рµ РЅРµСЃРѕРѕС‚РІРµС‚СЃС‚РІРёСЏ РІС…РѕРґРЅС‹С… РґР°РЅРЅС‹С… С„РѕСЂРјР°С‚Сѓ, РѕРЅРё РЅРµ РґРѕР±Р°РІР»СЏСЋС‚СЃСЏ РІ РјРЅРѕР¶РµСЃС‚РІРѕ
    
    s2 = MySet("FF02:0000:0000:0000:0000:0001:FF00:0300", "2001:db8:1234:0000:0000:0000:0000:0000")
    assert s2.set == {"FF02:0000:0000:0000:0000:0001:FF00:0300", "2001:db8:1234:0000:0000:0000:0000:0000"}


@pytest.fixture()
def make_default_set():
    s = MySet("FF02:0000:0000:0000:0000:0001:FF00:0300", "2001:db8:1234:0000:0000:0000:0000:0000")
    yield s

def test_add(make_default_set):
    make_default_set.add("1050:0000:0000:0000:0005:0600:300c:326b") 
    assert make_default_set.set == {"FF02:0000:0000:0000:0000:0001:FF00:0300", "1050:0000:0000:0000:0005:0600:300c:326b", "2001:db8:1234:0000:0000:0000:0000:0000"}
    
    with pytest.raises(TypeError):
        make_default_set.add(4)
        assert make_default_set.set == {"FF02:0000:0000:0000:0000:0001:FF00:0300", "1050:0000:0000:0000:0005:0600:300c:326b", "2001:db8:1234:0000:0000:0000:0000:0000"}


@pytest.fixture()
def make_set_with_a_history():

    s = MySet("FF02:0000:0000:0000:0000:0001:FF00:0300", "2001:db8:1234:0000:0000:0000:0000:0000")
    s.add("2001:0db8:0058:0074:1c60:2c18:1025:2b5f")
    s.add(["2001:DB8:0:0:0:0:0:1", "2001:0db8:2023:0910:60e1:76bd:601e:fa2f"])
    s.remove("2001:0db8:0058:0074:1c60:2c18:1025:2b5f")

    # s.undo_stack = [{'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:db8:1234:0000:0000:0000:0000:0000'}, {'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:0db8:0058:0074:1c60:2c18:1025:2b5f', '2001:db8:1234:0000:0000:0000:0000:0000'}, {'2001:0db8:0058:0074:1c60:2c18:1025:2b5f', '2001:db8:1234:0000:0000:0000:0000:0000', 'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:DB8:0:0:0:0:0:1', '2001:0db8:2023:0910:60e1:76bd:601e:fa2f'}]
    # s.set = {'2001:db8:1234:0000:0000:0000:0000:0000', 'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:DB8:0:0:0:0:0:1', '2001:0db8:2023:0910:60e1:76bd:601e:fa2f'}
    # s.redo_stack = []
    yield s


def test_undo(make_set_with_a_history):
    make_set_with_a_history.undo()
    assert make_set_with_a_history.set == {'2001:db8:1234:0000:0000:0000:0000:0000', 'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:DB8:0:0:0:0:0:1', '2001:0db8:2023:0910:60e1:76bd:601e:fa2f', "2001:0db8:0058:0074:1c60:2c18:1025:2b5f"}
    assert make_set_with_a_history.redo_stack == [{'2001:db8:1234:0000:0000:0000:0000:0000', 'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:DB8:0:0:0:0:0:1', '2001:0db8:2023:0910:60e1:76bd:601e:fa2f'}]
    assert make_set_with_a_history.undo_stack == [{'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:db8:1234:0000:0000:0000:0000:0000'}, {'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:0db8:0058:0074:1c60:2c18:1025:2b5f', '2001:db8:1234:0000:0000:0000:0000:0000'}]
    
    make_set_with_a_history.undo()
    make_set_with_a_history.undo()
    with pytest.raises(UndoError):
        make_set_with_a_history.undo()

def test_redo(make_set_with_a_history):
    with pytest.raises(RedoError):
        make_set_with_a_history.redo()
    make_set_with_a_history.undo()
    make_set_with_a_history.redo()

    assert make_set_with_a_history.set == {'2001:db8:1234:0000:0000:0000:0000:0000', 'FF02:0000:0000:0000:0000:0001:FF00:0300', '2001:DB8:0:0:0:0:0:1', '2001:0db8:2023:0910:60e1:76bd:601e:fa2f'}
