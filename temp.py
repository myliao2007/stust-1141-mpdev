import requests
import json

API_KEY = 'your api key'
url = "https://opendata.cwa.gov.tw/api/v1/rest/datastore/F-D0047-091"
params = {
    "Authorization": API_KEY,
    "LocationName": "臺南市",
    "ElementName": "最高溫度"
}
headers = {
    "accept": "application/json"
}

response = requests.get(url, params=params, headers=headers)
data = response.json()

print(json.dumps(data, indent=2, ensure_ascii=False))

