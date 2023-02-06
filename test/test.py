#!/usr/bin/python3

import chvi

def test_chvi(capfd):
    chvi.chvi("World")
    captured = capfd.readouterr()
    assert captured.out == "Hello, World!\n"

def test_elevation():
    assert chvi.elevation() == 21463
