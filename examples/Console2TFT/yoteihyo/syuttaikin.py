#!/usr/bin/python
# coding: utf8
import sys, telnetlib, urllib2
from . import userdata

if len(sys.argv) <= 2:
    print 'Usage: python syuttaikin.py <email> <url>'
    quit()
email = sys.argv[1]
url = sys.argv[2]

# Linino ONE bridge to Arduino
tn = telnetlib.Telnet('localhost', 6571)

def move_cursor(email):
    """行動予定表内で、指定した人の位置にカーソルを移動"""
    if email not in userdata.userdata:
        return False
    tn.write('R{}\n'.format(userdata.userdata[email][1]))
    tn.write('C0\n')
    #print('R{}\n'.format(userdata.userdata[email][1]))
    #print('C0\n')
    return True

def drawsyuttaikin(online):
    if move_cursor(email) is False:
        return
    if online:
        tn.write('c0\nb31\n') # text color black, bgcolor blue
        #print('c0\nb31\n') # text color black, bgcolor blue
    else:
        tn.write('c0\nb63488\n') # text color black, bgcolor red
        #print('c0\nb63488\n') # text color black, bgcolor red
    text = u'T' + userdata.userdata[email][0]
    sjis = text.encode("ms932")
    #sjis += ' ' * (7 - len(sjis))
    sjis += '\n'
    tn.write(sjis)
    #print(sjis)
    tn.write('b65535\n')

def geturl(url):
    resbody = None
    try:
        r = None
        r = urllib2.urlopen(url, timeout=60)
        resbody = r.read()
    except Exception, err:
        print 'HTTP error: ', err
    finally:
        if r: r.close()
    return resbody

def main():
    try:
        stat = geturl(url).rstrip()
        if stat == 'online':
            drawsyuttaikin(True)
        elif stat == 'offline':
            drawsyuttaikin(False)
        else:
            print 'unknown status: ' + stat
    finally:
        tn.close()
        #pass

if __name__ == "__main__":
    main()
