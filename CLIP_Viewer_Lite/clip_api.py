import requests
import json
import datetime

# Get current datetime
current_datetime = datetime.datetime.now()
now_datetime = current_datetime.strftime("%Y-%m-%d %H:%M:%S")

PATH_CONFIG = "CLIP_Viewer_Lite/clip_api_conf.json"

# Read clip_api_conf.json
with open(PATH_CONFIG, "r") as config_file:
    config_data = json.load(config_file)

device_id = config_data["device_id"]
api_url = config_data["api_url"]
headers = config_data["headers"]
body = config_data["body"]
is_debug = config_data["debug"]

elements = [
    "temperature",
    "humidity",
    "carbonDioxide",
    "xAcceleration",
    "yAcceleration",
    "zAcceleration",
    "pitchGyro",
    "rollGyro",
    "yawGyro",
    "airPressure",
    "pressure",
    "illuminance",
    "distance",
    "voltageInfo",
    "error",
    "chNumber",
    "trigger1",
    "trigger2",
]


def debug_print(data):
    if is_debug:
        print(data)


def GET_token():
    # Get token by POST request
    response = requests.post(
        f"{api_url}/auth/token", headers=headers, data=json.dumps(body)
    )

    # Get token from response body
    token = response.json()["token"]
    debug_print("Get token: " + response.json()["result"])

    return token


def GET_API_data():
    # Set token in headers for next API access
    headers["Authorization"] = GET_token()

    # Get test data by GET request
    response = requests.get(f"{api_url}/payload/latest/{device_id}", headers=headers)

    # Check response
    debug_print("Get API_data: " + response.text)

    # Parse response JSON
    select_data = {}
    data = response.json()

    if config_data["data_selection"]["All"]:
        return data["payload"][0]

    for element in elements:
        if config_data["data_selection"][element]:
            select_data[element] = data["payload"][0][element]

    return select_data

    
def POST_API_data(token):
    # Set token in headers for next API access
    headers["Authorization"] = GET_token()
    
    # Test data
    test_data = {
        "deviceId": device_id,
        "sendDateTime": now_datetime,
        "payloadType": "Temperature and humidity information",
        "payload": "36.5,80",
        "communicationLine": "LTE",
        "rssi": -85,
    }

    # Send test data by POST request
    response = requests.post(f"{api_url}/payload/send", headers=headers, data=json.dumps(test_data))
    debug_print("POST testdata:" + response.text)

def main():
    print(GET_API_data())

if __name__ == "__main__":
    main()