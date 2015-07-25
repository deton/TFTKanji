#!/usr/bin/python
# QUERY_STRING='text=c-1%0ab0%0aR3%0aC1%0aTtest' python tftkanji.py
import cgi
import sys, telnetlib

def send(text):
    try:
        tn = None
        tn = telnetlib.Telnet('localhost', 6571)
        for line in text.split('\n'):
            print 'W ' + line
            tn.write(line + '\n')
            buf = ''
            while buf == '':
                buf = tn.read_until('> ') # ready?
                print 'R "' + buf + '"'
    finally:
        if tn: tn.close()

def main():
    print "Content-Type: text/plain"
    print

    form = cgi.FieldStorage()
    text = form.getfirst("text").decode("utf-8").encode("ms932")
    send(text)
    sys.stdout.flush()
    
if __name__ == "__main__":
    main()
