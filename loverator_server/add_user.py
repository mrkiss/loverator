import sys
import os
sys.path.insert(0, ',')
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "vibrator.settings")
import django
from vibrator import settings
#from django.core.management import setup_environ
from django.contrib.auth.models import User
from ping.models import *
#from django.core import management

#setup_environ(settings)
#management.settings = settings
django.setup()

for item in range(1,11):
    user  = User.objects.create_user(str(item), str(item)+ 'test@daum.net', str(item)+str(item))
    user.is_active = True
    user.save()
    p = Person()
    p.user = user
    p.number = str(item)
    p.save()

