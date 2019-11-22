#import tweepy
from TwitterAPI import TwitterAPI
import matplotlib.pyplot as plt
import csv
import os
import requests
import numpy as np


import matplotlib.pyplot as plt 
import json
import requests


response = requests.get("https://api.thingspeak.com/channels/864235/fields/1?api_key=HKJF50PJ4NF1ZGC2")
todos = json.loads(response.text)

#print(todos['feeds'])

print(todos)
j = 0 
'''
URL = "http://api.thingspeak.com/channels/864235/feed.csv?results=8000"
download = requests.get(URL).content.decode('utf-8')#.split("\n",1)[1]

jj=0
dict1=[]
dict2=[]
dict3=[]
arr=[]
an=[]
arey=0

for i in download.split('\n'):
    try:
        j = i.split(',')
        arey = arey + 1
    except:
        print("Gi")

for i in download.split('\n'):
    try:
        an.append(jj)
        j = i.split(',')
        print(j[0])
        try:
            arr.append(j[0])
        except:
            print("RaRa")
        try:
            a=j[1]
            b=j[2]
            c=j[3]
            dict1.append(float(a))
            dict2.append(float(b))
            dict3.append(float(c))
        except:
            print("Anna")

        jj = jj + 1
        if jj > 4:
            break
    except:
        break
    
print(arr)
x=np.array([1,2,3])
a=[y for y in dict1]
print(a)
plt.xticks(x,arr)
plt.plot(x,a)
plt.show()
'''


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
