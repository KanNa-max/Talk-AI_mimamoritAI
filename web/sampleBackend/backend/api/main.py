from fastapi import Depends, FastAPI, HTTPException, Security
from fastapi.security.api_key import APIKeyHeader, APIKey
from starlette.middleware.cors import CORSMiddleware # 追加
from starlette.status import HTTP_403_FORBIDDEN

from routers import test
from util import util

correct_key: str = util.get_apikey()
api_key_header = APIKeyHeader(name='Authorization', auto_error=False)

async def get_api_key(
    api_key_header: str = Security(api_key_header),
    ):
    if api_key_header == correct_key:
        return api_key_header
    else:
        raise HTTPException(
            status_code=HTTP_403_FORBIDDEN, detail="Could not validate credentials"
        )

app = FastAPI()
app.include_router(test.router, dependencies=[Depends(get_api_key)], tags=["Test"])
# CORSを回避するために追加（今回の肝）
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:8080", "*"],
    allow_credentials=True,   # 追記により追加
    allow_methods=["*"],      # 追記により追加
    allow_headers=["*"]       # 追記により追加
)

# fetch
import requests
import json
import datetime

PATH_CONFIG = "clip_api_conf.json"

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

    print(response.json())
    # Get token from response body
    token = response.json()["token"]
    debug_print("Get token: " + response.json()["result"])

    return token

async def GET_DAYS(From, To):
    current_datetime = datetime.datetime.now()
    from_datetime = (current_datetime - datetime.timedelta(To-1)).strftime("%Y%m%dT")
    to_datetime = (current_datetime - datetime.timedelta(From)).strftime("%Y%m%dT")
    get_url = f"{api_url}/payload/{device_id}?from={from_datetime}000000&to={to_datetime}235959"
    print(get_url)

    dates = [(current_datetime - datetime.timedelta(i)).strftime("%Y,%m,%d").split(",") for i in range(From, To)]

    # Get test data by GET request
    response = requests.get(get_url, headers=headers)

    # Check response
    debug_print("Get API_data: " + response.text)

    # Parse response JSON
    data = {
        "payload": response.json()["payload"],
        "date": dates
    }
    return data

async def GET_API_data(isDays):
    # Set token in headers for next API access
    headers["Authorization"] = GET_token()

    data = {"day03": await GET_DAYS(0, 3)}
    if not isDays:
        data["day35"] = await GET_DAYS(3, 5)
        data["day57"] = await GET_DAYS(5, 7)
    return {"getData": data}

def main(day):
    return GET_API_data(day==3)

@app.get("/3")
async def read_root():
    return await main(3)

@app.get("/7")
async def read_root():
    return await main(7)

@app.get("/")
async def read_root():
    return {"status":"coneccted"}
