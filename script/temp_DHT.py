import matplotlib.pyplot as plt
import json
import requests


response = requests.get("https://api.thingspeak.com/channels/864235/fields/1?api_key=HKJF50PJ4NF1ZGC2")
todos = json.loads(response.text)

#print(todos['feeds'])

j = 0
for i in todos['feeds']:
    print(i['created_at'],end="")
    print(",",end="")
    print(i['field1'])