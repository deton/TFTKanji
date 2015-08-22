#!/usr/bin/python
# coding: utf8
import json, urllib2, sys, time, datetime, telnetlib, re
from . import userdata, ignore_patterns, location

if len(sys.argv) <= 1:
    print 'Usage: python yoteihyo.py <url>'
    quit()
url = sys.argv[1]

# Linino ONE bridge to Arduino
tn = telnetlib.Telnet('localhost', 6571)

def init():
    tn.write('c0\nb65535\n') # text color black, bgcolor white
    #print('c0\nb65535\n') # text color black, bgcolor white

def draw_text(text):
    # to shift_jis
    sjis = text.encode("ms932")
    tn.write(sjis)
    #print(sjis)

def fetch_yoteihyo():
    yoteihyo = None
    try:
        r = None
        r = urllib2.urlopen(url, timeout=60)
        yoteihyo = json.loads(r.read())
    except Exception, err:
        print '{} HTTP error: {}'.format(datetime.datetime.now(), err)
        sys.stdout.flush()
    finally:
        if r: r.close()
    return yoteihyo

def move_cursor(user):
    """行動予定表内で、指定した人の位置にカーソルを移動"""
    if user not in userdata.userdata:
        return False
    tn.write('R{}\n'.format(userdata.userdata[user][1]))
    tn.write('C{}\n'.format(userdata.userdata[user][2]))
    tn.write('K\n')
    #print('R{}\n'.format(userdata.userdata[user][1]))
    #print('C{}\n'.format(userdata.userdata[user][2]))
    #print('K\n')
    return True

def is_ignore(subject):
    for pat in ignore_patterns.ignore_patterns:
        if re.search(pat, subject):
            return True
    return False

def shorten_location(loc):
    for pat in location.location.viewkeys():
        loc = re.sub(pat, location.location[pat], loc)
    return loc

def draw_tft(yoteihyo):
    now = datetime.datetime.today()
    # {"taro@example.com":[{"startTime":1438644354,"endTime":1438644354,
    #   "subject":"会議","location":"roomA","freeBusyStatus":"Busy"},...],...}
    for user in yoteihyo.viewkeys():
        if move_cursor(user) is False:
            continue
        prev = now
        headmark = u'▼'
        for e in yoteihyo[user]:
            start = datetime.datetime.fromtimestamp(e[u'startTime'])
            end   = datetime.datetime.fromtimestamp(e[u'endTime'])
            # 終了予定後、2時間経過している予定は無視。
            # 終わらず続いている場合は知りたい。
            if end + datetime.timedelta(hours=2) < now:
                #print 'ended event: {} + 2 hour < {}'.format(end, now)
                continue
            subj = e[u'subject']
            if subj is None:
                subj = '-'
            if e[u'freeBusyStatus'] == u'Free' or is_ignore(subj):
                #print 'ignore'
                continue
                
            need_reset_color = False
            if e[u'freeBusyStatus'] == u'Tentative': # 仮の予定
                draw_text(u'c31727\n') # gray
                need_reset_color = True
            elif e[u'freeBusyStatus'] == u'OOF': # 外出中
                draw_text(u'c63519\n') # magenta
                need_reset_color = True

            outstr = u'T'
            if start.date().day != prev.date().day or start.date().month != prev.date().month:
                if start.date().day == now.date().day and start.date().month == now.date().month:
                    # 日全体の予定があって翌日終了のため●になったのを戻す
                    headmark = u'▼'
                else:
                    headmark = u'●'
                outstr += headmark
                if start.date().month != prev.date().month:
                    outstr += u'{0:%m}月{0:%d}日'.format(start)
                else:
                    outstr += u'{:%d}日'.format(start)
            else:
                outstr += headmark
            prev = start

            outstr += start.strftime(u'%H:%M-')

            if end.date().day != prev.date().day:
                outstr += u'{:%d}日'.format(end)
                if end.date().day == now.date().day and end.date().month == now.date().month:
                    headmark = u'▼'
                else:
                    headmark = u'●'
            prev = end
            outstr += end.strftime(u'%H:%M')
            outstr += subj
            if e[u'location'] is not None:
                outstr += u'('
                outstr += shorten_location(e[u'location'])
                outstr += u')'
            if need_reset_color:
                outstr += u'\nc0' # black
            outstr += u'\n'
            draw_text(outstr)

def main():
    try:
        init()
        yoteihyo = fetch_yoteihyo()
        if yoteihyo:
            draw_tft(yoteihyo)
    finally:
        tn.close()
        #pass

if __name__ == "__main__":
    main()
