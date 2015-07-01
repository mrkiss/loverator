from django.conf.urls import include, url
from django.contrib import admin



urlpatterns = [
    # Examples:
    # url(r'^$', 'vibrator.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^$', 'ping.views.home', name='home'),
    url(r'^accounts/login/$', 'ping.views.ping_login', name='ping_login'),
    url(r'^accounts/logout/$', 'ping.views.ping_logout', name='ping_logout'),
    url('', include('django.contrib.auth.urls')),
    url(r'^accounts/profile/$', 'ping.views.gohome', name='ping_gohome'), 
]

