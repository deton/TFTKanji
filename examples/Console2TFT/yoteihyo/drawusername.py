#!/usr/bin/python
# coding: utf8
import sys, telnetlib
from . import userdata

# Linino ONE bridge to Arduino
tn = telnetlib.Telnet('localhost', 6571)

def init():
    tn.write('c0\nb65535\n') # text color black, bgcolor white
    #print('c0\nb65535\n') # text color black, bgcolor white


def move_cursor(user):
    """行動予定表内で、指定した人の位置にカーソルを移動"""
    tn.write('R{}\n'.format(userdata.userdata[user][1]))
    tn.write('C0\n')
    #print('R{}\n'.format(userdata.userdata[user][1]))
    #print('C0\n')

def draw_tft():
    for user in userdata.userdata.viewkeys():
        move_cursor(user)
        # XXX: width of utf-8 char is 1
        # draw_text(u'T{:6}\n'.format(userdata.userdata[user][0]))
        text = u'T' + userdata.userdata[user][0]
        sjis = text.encode("ms932")
        #sjis += ' ' * (7 - len(sjis))
        sjis += '\n'
        tn.write(sjis)
        #print(sjis)

def main():
    try:
        draw_tft()
    finally:
        tn.close()
        #pass

if __name__ == "__main__":
    main()
