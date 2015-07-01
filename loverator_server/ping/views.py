from django.shortcuts import render
from ping.models import *
from django.contrib.auth.decorators import login_required
from django.contrib.auth import authenticate, login, logout
from django.shortcuts import render_to_response, redirect
from django.template import RequestContext as RC
from ping_forms import PingLoginForm
from ping.models import *
import os

import urllib

# Create your views here.

@login_required
def home(rq):
    if rq.POST:
        pnumber = rq.POST['person']
        pto = Person.objects.filter(number=pnumber)[0]
        pfrom = Person.objects.get(user = rq.user)
        pl = PingLog()
        pl.pingfrom = pfrom
        pl.pingto = pto
        pl.save()
        
        persons = Person.objects.all()
        recent_number=pnumber
        if reciprocal_check(pto, pfrom):
            os.system('/home/pi/w/send_signal.py %ss'%(pto.number))
            os.system('/home/pi/w/send_signal.py %ss'%(pfrom.number))
            #urllib.urlopen('http://192.168.43.65/?node=%ss'%(pto.number))
            #urllib.urlopen('http://192.168.43.65/?node=%ss'%(pfrom.number))
            
            return render_to_response('home.html', RC(rq, {'persons':persons, 'next':'/', 'msg':'connected!!!', 'recent_number':recent_number}))

        else:
            #urllib.urlopen('http://192.168.43.65/?node=%sw'%(pto.number))
            os.system('/home/pi/w/send_signal.py %sw'%(pto.number))
            return render_to_response('home.html', RC(rq, {'persons':persons, 'next':'/', 'recent_number':recent_number}))
    else :
        recent_number=''
        persons = Person.objects.all()
        return render_to_response('home.html', RC(rq, {'persons':persons, 'next':'/', 'recent_number':recent_number}))

def reciprocal_check(pto , pfrom):
    forward = PingLog.objects.filter( pingto=pto, pingfrom=pfrom).order_by('-pingtime')
    reverse = PingLog.objects.filter( pingto=pfrom, pingfrom=pto).order_by('-pingtime')
    if forward and reverse :
        if PingLog.objects.filter(pingfrom=pto).order_by('-pingtime')[0].pingto == pfrom:

            f_time = forward[0].pingtime
            t_time = reverse[0].pingtime
        #if 60 > (f_time - t_time).seconds  >-60 : 
            if  (f_time - t_time).seconds < 60 : 
                return True
    return False 


def gohome(rq):
    return redirect('home')
    

def ping_login(rq):
    if rq.POST:
        username=rq.POST['username']
        password = rq.POST['password']
        user = authenticate(username=username, password=password)
        if user is not None:
            login (rq, user)
            next_url = 'home'
            if rq.GET.has_key('next'):
                next_url =  rq.GET['next']
            return redirect(next_url)
        else:
            return redirect('login')
    else :
        return render_to_response('registration/ping_login.html', RC(rq, {'form':PingLoginForm()}))
    

@login_required
def ping_logout(rq):
    logout(rq)
    return redirect('home')
