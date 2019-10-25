import matplotlib.pyplot as plt
import json
import requests


response = requests.get("https://api.thingspeak.com/channels/864235/fields/1?api_key=ZCQ283T78EAFLBXG")
todos = json.loads(response.text)

#print(todos['feeds'])

j = 0
for i in todos['feeds']:
    j+=1
    print(i['created_at'],end="")
    print(",",end="")
    print(i['field1'])
