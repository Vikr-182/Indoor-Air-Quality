#import tweepy
from TwitterAPI import TwitterAPI
import matplotlib.pyplot as plt
import csv
import os
import requests
import numpy
import statistics

URL = "http://api.thingspeak.com/channels/864235/feed.csv?results=8000"
download = requests.get(URL).content.decode('utf-8')#.split("\n",1)[1]
#print(download.split(','))

jj=0
dict1={}
dict2={}
dict3={}
for i in download.split('\n'):
    print(i.split(','))
    j = i.split(',')
    dict1[j[0]] = j[1]
    dict2[j[0]] = j[2]
    dict3[j[0]] = j[3]
    print(dict1[j[0]])
    jj = jj + 1


'''
consumer_key = 'TynhLBXGxZepWH0WQVf26QdpV'
consumer_secret = 'n7hBsbrQXUTmRgps9pmPYNl85OKy3qyHd15fOZ7W7TD6lM3Rek'
access_token = '1192421889815105536-B6osx8nZSbh6oi7ORt5ydybBZMVpSL'
access_token_secret = 'se2unVv40EJajV3CJMYJnhrFQ43zcJP4ncPX5DtBrQVgI'

api = TwitterAPI(consumer_key,
        consumer_secret,
        access_token,
        access_token_secret)

r = api.request('statuses/update', {'status': str(data)})
print( 'SUCCESS' if r.status_code == 200 else 'FAILURE' )
'''
