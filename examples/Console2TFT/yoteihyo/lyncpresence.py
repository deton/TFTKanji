#!/usr/bin/python
# coding: utf8
import sys, telnetlib, urllib2, json, datetime
from . import userdata

if len(sys.argv) <= 1:
    print 'Usage: python lyncpresence.py <url>'
    quit()
url = sys.argv[1]

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

def drawlyncpresence(email, presence):
    if move_cursor(email) is False:
        return
    if presence in ['Away', 'BeRightBack']:
        tn.write('c0\nb65504\n') # text color black, bgcolor yellow
        #print('c0\nb65504\n')
    elif presence in ['Busy', 'DoNotDisturb', 'IdleBusy']:
        tn.write('c0\nb63488\n') # text color black, bgcolor red
        #print('c0\nb63488\n')
    elif presence in ['Online', 'IdleOnline']:
        tn.write('c0\nb2016\n') # text color black, bgcolor green
        #print('c0\nb2016\n')
    elif presence in ['Offline']:
        tn.write('c0\nb54938\n') # text color black, bgcolor light grey
        #print('c0\nb54938\n')
    else:
        tn.write('c0\nb65535\n') # text color black, bgcolor white
        #print('c0\nb65535\n')
    text = u'T' + userdata.userdata[email][0]
    sjis = text.encode("ms932")
    #sjis += ' ' * (7 - len(sjis))
    sjis += '\n'
    tn.write(sjis)
    #print(sjis)
    tn.write('b65535\n')

def geturl(url):
    presences = None
    try:
        r = None
        r = urllib2.urlopen(url, timeout=60)
        presences = json.loads(r.read())
    except Exception, err:
        print '{} HTTP error: {}'.format(datetime.datetime.now(), err)
        sys.stdout.flush()
    finally:
        if r: r.close()
    return presences

def main():
    try:
        presences = geturl(url)
        if presences:
            # {"taro@example.com":"Away", "jiro@example.com":"Online", ...}
            for email in presences.viewkeys():
                drawlyncpresence(email, presences[email])
    finally:
        tn.close()
        #pass

if __name__ == "__main__":
    main()
