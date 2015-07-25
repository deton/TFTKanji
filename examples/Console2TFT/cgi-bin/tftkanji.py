#!/usr/bin/python
import cgi
import sys, telnetlib

class McuConnection(object):
    def send(self, text):
        tn = telnetlib.Telnet('localhost', 6571)
        for line in text:
            str = line.strip()
            print 'W ' + str
            tn.write(str + '\n')
            buf = ''
            while buf == '':
                buf = tn.read_until('> ') # ready?
                print 'R "' + buf + '"'
                sys.stdout.flush()
        tn.close()

conn = McuConnection()

def main():
    print "Content-Type: text/plain"
    print

    form = cgi.FieldStorage()
    text = form.getfirst("text").decode("utf-8")
    # TODO: Shift_JIS
    conn.send(text)

if __name__ == "__main__":
    conn.send(sys.stdin.readlines())
