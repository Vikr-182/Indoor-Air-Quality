#import tweepy
from TwitterAPI import TwitterAPI
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import csv
import os
import requests
import numpy as np
import json
import requests
import datetime
from datetime import timedelta

response = requests.get("https://api.thingspeak.com/channels/864607/fields/1?api_key=HKJF50PJ4NF1ZGC2")
todos = json.loads(response.text)

#print(todos['feeds'])

#print(todos)
j = 0
an=[]
pr=[]
cs=[]
isnight=0
ismorning=0
isafternoon=0
isdusk=0
isdawn=0
startdate=0
enddate=0

num=0

for i in todos['feeds']:
    num = num + 1

k=0
for i in todos['feeds']:
    j = j + 1
    string = i['created_at']
    alpha = string.split("T")[1]
    rama = string.split("T")[0]
    beta = alpha.split("Z")[0]
    dateobject = datetime.datetime.strptime(beta,'%H:%M:%S')
    rama = rama + " " + beta
    dateobject2 = datetime.datetime.strptime(rama,'%Y-%m-%d %H:%M:%S')
    if k is 0:
        startdate = dateobject2 + timedelta(hours=5)+timedelta(minutes=30) 
    if k is num-1:
        enddate = dateobject2 + timedelta(hours=5)+timedelta(minutes=30)
    k = k + 1
    cs.append(j)
    an.append(str(dateobject2 + timedelta(hours=5)+timedelta(minutes=30)))
    pr.append(i['field1'])


datatypes = [('date','S20'),('col','float32')]
li = [float(x) for x in pr]
print(np.mean(li))
mean=np.mean(li)
variance=np.var(li)
high=max(li)
low=min(li)

print(mean)
print(variance)
print(high)
print(low)
print(str(startdate))
print(str(enddate))

data_tuples = [(an[i],pr[i]) for i in range(0,len(an))]
data = np.array(data_tuples,dtype=datatypes)
col1 = data['col']
dates = mdates.num2date(mdates.datestr2num(data['date']))
plt.xlabel('Time')
fig,ax1 = plt.subplots()
ax1.set_title('PM2.5 Concentration')
ax1.set(xlabel='Time',ylabel='PM2.5 in ppm')
#img=plt.imread("afternoon.jpeg")
#ax1.imshow(img,extent=[0,100,0,100])
#ax1.ylabel({'PM2.5','(in ppm)'})
ax1.plot(dates,col1)
fig.autofmt_xdate()
#plt.show()
plt.savefig('foo.png')


consumer_key = 'TynhLBXGxZepWH0WQVf26QdpV'
consumer_secret = 'n7hBsbrQXUTmRgps9pmPYNl85OKy3qyHd15fOZ7W7TD6lM3Rek'
access_token = '1192421889815105536-B6osx8nZSbh6oi7ORt5ydybBZMVpSL'
access_token_secret = 'se2unVv40EJajV3CJMYJnhrFQ43zcJP4ncPX5DtBrQVgI'

api = TwitterAPI(consumer_key,
        consumer_secret,
        access_token,
        access_token_secret)

message=0
message="Start Time:\t         "+str(startdate)
message+= "\nEnd Time:\t        " + str(enddate)
message+= "\nMax Value:\t      " + str(high)
message+= "\nMin Value:\t      " + str(low)
message+= "\nAverage:\t         "+str(mean)
message+= "\nVariance:\t         "+str(variance)
message+= "\n"

file=open('foo.png','rb')
if file is None:
    print("Eror")
else:
    data=file.read()
    r = api.request('statuses/update_with_media', {'status': str(message)},{'media[]':data})
    print( 'SUCCESS' if r.status_code == 200 else 'FAILURE' )
