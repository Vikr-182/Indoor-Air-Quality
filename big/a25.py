import csv
import requests

URL='http://api.thingspeak.com/channels/864607/feed.csv?results=8000'
with requests.Session() as s:
    download = s.get(URL)
    decode = download.content.decode('utf-8')
    print(decode)

