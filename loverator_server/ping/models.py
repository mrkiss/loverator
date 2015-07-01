from django.db import models
from django.contrib.auth.models import User, Group, Permission


class Person(models.Model):    
    user = models.OneToOneField( User )
    number = models.CharField(max_length=3)

class PingLog(models.Model):
    pingfrom = models.ForeignKey('Person', related_name='pingfrom')
    pingto = models.ForeignKey('Person', related_name='pingto')
    pingtime  = models.DateTimeField(auto_now_add=True)

