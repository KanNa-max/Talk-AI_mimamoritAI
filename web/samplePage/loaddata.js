const is_debug = true;
const gradientDeg = (LOAD_DAYS==3)? "to bottom":"to right";
const pointAxis = (LOAD_DAYS==3)? "top":"left";

const elements = [
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

const colors = (LOAD_DAYS==3)? 
    [
        "#ffffff",
        "#ffffff",
        "#ffffff"
    ]:
    [
        "#FF61BB",
        "#ff62b3",
        "#ff63ab",
        "#ff63a2",
        "#ff649b",
        "#ff6592",
        "#FF658A"
    ];

class TimeTable{
    constructor(n=0, s=0, a=0){
        this.n = n;
        this.s = s;
        this.a = a;
    }
}

const sampleData = [
    // 0日前
    [
        [...Array(6)].map(() => new TimeTable()), //0
        [...Array(6)].map(() => new TimeTable()), //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [...Array(6)].map(() => new TimeTable()), //6
        [...Array(6)].map(() => new TimeTable()), //7
        [...Array(6)].map(() => new TimeTable()), //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [new TimeTable(), new TimeTable(), new TimeTable(), new TimeTable(5,1,0), new TimeTable(10,0,0), new TimeTable(8,1,0),], //11
        [new TimeTable(5,1,0), new TimeTable(6,1,0), new TimeTable(), new TimeTable(), new TimeTable(), new TimeTable(),], //12
        [...Array(6)].map(() => new TimeTable()), //13
        [...Array(6)].map(() => new TimeTable()), //14
        [...Array(6)].map(() => new TimeTable()), //15
        [...Array(6)].map(() => new TimeTable()), //16
        [...Array(6)].map(() => new TimeTable()), //17
        [...Array(6)].map(() => new TimeTable()), //18
        [...Array(6)].map(() => new TimeTable()), //19
        [...Array(6)].map(() => new TimeTable()), //20
        [...Array(6)].map(() => new TimeTable()), //21
        [...Array(6)].map(() => new TimeTable()), //22
        [...Array(6)].map(() => new TimeTable()), //23
    ],
    // 1日前
    [
        [new TimeTable(8,5,0), new TimeTable(8,1,0), new TimeTable(3,0,0), new TimeTable(4,1,0), new TimeTable(3,0,0), new TimeTable(7,1,0),], //0
        [new TimeTable(8,2,0), new TimeTable(8,1,0), new TimeTable(4,3,1), new TimeTable(4,2,0), new TimeTable(5,2,0), new TimeTable(7,4,0),], //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [...Array(6)].map(() => new TimeTable()), //6
        [...Array(6)].map(() => new TimeTable()), //7
        [...Array(6)].map(() => new TimeTable()), //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [new TimeTable(), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //11
        [new TimeTable(5,1,0), new TimeTable(6,3,0), new TimeTable(8,5,0), new TimeTable(), new TimeTable(), new TimeTable(),], //12
        [new TimeTable(), new TimeTable(5,3,0), new TimeTable(), new TimeTable(), new TimeTable(6,0,0), new TimeTable(7,0,0),], //13
        [new TimeTable(6,0,0), new TimeTable(7,4,0), new TimeTable(6,1,0), new TimeTable(7,4,0), new TimeTable(4,2,0), new TimeTable(),], //14
        [...Array(6)].map(() => new TimeTable()), //15
        [...Array(6)].map(() => new TimeTable()), //16
        [...Array(6)].map(() => new TimeTable()), //17
        [...Array(6)].map(() => new TimeTable()), //18
        [...Array(6)].map(() => new TimeTable()), //19
        [...Array(6)].map(() => new TimeTable()), //20
        [new TimeTable(), new TimeTable(1,0,0), new TimeTable(6,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(7,1,0),], //21
        [new TimeTable(7,0,0), new TimeTable(2,0,0), new TimeTable(6,0,0), new TimeTable(10,1,0), new TimeTable(), new TimeTable(),], //22
        [...Array(6)].map(() => new TimeTable()), //23
    ],
    // 2日前
    [
        [...Array(6)].map(() => new TimeTable()), //0
        [...Array(6)].map(() => new TimeTable()), //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [...Array(6)].map(() => new TimeTable()), //6
        [...Array(6)].map(() => new TimeTable()), //7
        [...Array(6)].map(() => new TimeTable()), //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [...Array(6)].map(() => new TimeTable()), //11
        [...Array(6)].map(() => new TimeTable()), //12
        [...Array(6)].map(() => new TimeTable()), //13
        [...Array(6)].map(() => new TimeTable()), //14
        [...Array(6)].map(() => new TimeTable()), //15
        [...Array(6)].map(() => new TimeTable()), //16
        [...Array(6)].map(() => new TimeTable()), //17
        [new TimeTable(0,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,7,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //18
        [new TimeTable(6,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //19
        [new TimeTable(6,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //20
        [new TimeTable(), new TimeTable(), new TimeTable(6,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(7,1,0),], //21
        [new TimeTable(7,0,0), new TimeTable(8,5,0), new TimeTable(6,0,0), new TimeTable(10,1,0), new TimeTable(8,0,0), new TimeTable(7,0,0),], //22
        [new TimeTable(7,1,0), new TimeTable(8,5,0), new TimeTable(3,0,0), new TimeTable(4,1,0), new TimeTable(5,0,0), new TimeTable(7,1,0),], //23
    ],
    // 3日前
    [
        [...Array(6)].map(() => new TimeTable()), //0
        [...Array(6)].map(() => new TimeTable()), //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [...Array(6)].map(() => new TimeTable()), //6
        [...Array(6)].map(() => new TimeTable()), //7
        [...Array(6)].map(() => new TimeTable()), //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [...Array(6)].map(() => new TimeTable()), //11
        [...Array(6)].map(() => new TimeTable()), //12
        [...Array(6)].map(() => new TimeTable()), //13
        [...Array(6)].map(() => new TimeTable()), //14
        [...Array(6)].map(() => new TimeTable()), //15
        [...Array(6)].map(() => new TimeTable()), //16
        [...Array(6)].map(() => new TimeTable()), //17
        [...Array(6)].map(() => new TimeTable()), //18
        [...Array(6)].map(() => new TimeTable()), //19
        [...Array(6)].map(() => new TimeTable()), //20
        [...Array(6)].map(() => new TimeTable()), //21
        [...Array(6)].map(() => new TimeTable()), //22
        [...Array(6)].map(() => new TimeTable()), //23
    ],
    // 4日目
    [
        [...Array(6)].map(() => new TimeTable()), //0
        [...Array(6)].map(() => new TimeTable()), //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [new TimeTable(), new TimeTable(6,2,0), new TimeTable(8,1,0), new TimeTable(7,0,0), new TimeTable(6,4,0), new TimeTable(1,0,0),], //6
        [new TimeTable(5,0,0), new TimeTable(6,2,0), new TimeTable(8,1,0), new TimeTable(), new TimeTable(), new TimeTable(4,1,0),], //7
        [new TimeTable(5,0,0), new TimeTable(6,0,0), new TimeTable(), new TimeTable(), new TimeTable(), new TimeTable(),], //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [...Array(6)].map(() => new TimeTable()), //11
        [...Array(6)].map(() => new TimeTable()), //12
        [...Array(6)].map(() => new TimeTable()), //13
        [new TimeTable(), new TimeTable(5,1,0), new TimeTable(6,0,0), new TimeTable(), new TimeTable(), new TimeTable(),], //14
        [new TimeTable(6,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //15
        [new TimeTable(6,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //16
        [...Array(6)].map(() => new TimeTable()), //17
        [...Array(6)].map(() => new TimeTable()), //18
        [...Array(6)].map(() => new TimeTable()), //19
        [...Array(6)].map(() => new TimeTable()), //20
        [...Array(6)].map(() => new TimeTable()), //21
        [...Array(6)].map(() => new TimeTable()), //22
        [...Array(6)].map(() => new TimeTable()), //23
    ],
    // 5日目
    [
        [...Array(6)].map(() => new TimeTable()), //0
        [...Array(6)].map(() => new TimeTable()), //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [...Array(6)].map(() => new TimeTable()), //6
        [...Array(6)].map(() => new TimeTable()), //7
        [...Array(6)].map(() => new TimeTable()), //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [...Array(6)].map(() => new TimeTable()), //11
        [...Array(6)].map(() => new TimeTable()), //12
        [...Array(6)].map(() => new TimeTable()), //13
        [...Array(6)].map(() => new TimeTable()), //14
        [...Array(6)].map(() => new TimeTable()), //15
        [...Array(6)].map(() => new TimeTable()), //16
        [...Array(6)].map(() => new TimeTable()), //17
        [...Array(6)].map(() => new TimeTable()), //18
        [...Array(6)].map(() => new TimeTable()), //19
        [new TimeTable(), new TimeTable(), new TimeTable(8,1,0), new TimeTable(7,0,0), new TimeTable(6,0,0), new TimeTable(7,0,0),], //20
        [new TimeTable(8,0,0), new TimeTable(), new TimeTable(8,1,0), new TimeTable(7,0,0), new TimeTable(6,0,0), new TimeTable(8,0,0),], //21
        [new TimeTable(8,0,0), new TimeTable(7,0,0), new TimeTable(8,1,1), new TimeTable(7,0,0), new TimeTable(8,0,0), new TimeTable(8,0,0),], //22
        [new TimeTable(8,0,0), new TimeTable(), new TimeTable(), new TimeTable(), new TimeTable(8,0,0), new TimeTable(8,5,0),], //23
    ],
    // 6日前
    [
        [...Array(6)].map(() => new TimeTable()), //0
        [...Array(6)].map(() => new TimeTable()), //1
        [...Array(6)].map(() => new TimeTable()), //2
        [...Array(6)].map(() => new TimeTable()), //3
        [...Array(6)].map(() => new TimeTable()), //4
        [...Array(6)].map(() => new TimeTable()), //5
        [...Array(6)].map(() => new TimeTable()), //6
        [...Array(6)].map(() => new TimeTable()), //7
        [...Array(6)].map(() => new TimeTable()), //8
        [...Array(6)].map(() => new TimeTable()), //9
        [...Array(6)].map(() => new TimeTable()), //10
        [...Array(6)].map(() => new TimeTable()), //11
        [...Array(6)].map(() => new TimeTable()), //12
        [new TimeTable(6,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //13
        [new TimeTable(6,0,0), new TimeTable(5,0,0), new TimeTable(10,0,0), new TimeTable(10,1,0), new TimeTable(10,0,0), new TimeTable(4,1,0),], //14
        [new TimeTable(), new TimeTable(), new TimeTable(8,1,0), new TimeTable(7,0,0), new TimeTable(6,0,0), new TimeTable(),], //15
        [...Array(6)].map(() => new TimeTable()), //16
        [...Array(6)].map(() => new TimeTable()), //17
        [...Array(6)].map(() => new TimeTable()), //18
        [...Array(6)].map(() => new TimeTable()), //19
        [...Array(6)].map(() => new TimeTable()), //20
        [new TimeTable(), new TimeTable(), new TimeTable(5,1,0), new TimeTable(6,0,0), new TimeTable(7,0,0), new TimeTable(),], //21
        [new TimeTable(7,4,0), new TimeTable(), new TimeTable(5,1,0), new TimeTable(6,3,0), new TimeTable(7,4,0), new TimeTable(),], //22
        [...Array(6)].map(() => new TimeTable()), //23
    ],
]

const parcent = {
    hours : [...Array(24)].map((_, i) => i*100/24),
    minutes : [...Array(6)].map((_, i) => i*100/24/6),
    p : function(hour, minute){
        return Math.round(100*(this.hours[hour] + this.minutes[minute]))/100;
    }
}

const debugLog = (data) => {
    if(is_debug) console.log(data);
};

const getPayload = async () => {
    try{
        const response = await fetch(`http://192.168.3.13:8000/${LOAD_DAYS}`);
        if ( !response.ok ){
            throw response;
        }
	    data = await response.json();
        return {
            success: true,
            data: data.getData
        };
    }catch( res ){
        debugLog(`Get Error\n status: ${res.status}\n${res.json.result}`);
        return {
            success: false,
            data: []
        };
    }
};

const loadingFeedOut = () => {
    const loadingElement = document.getElementById("loading");
    loadingElement.classList.add('fadeout');
    setTimeout(function(){ 
        loadingElement.style.display = "none"; 
    }, 1000);
}

const makeTimeber = (day, from, to) => {
    for ( let i=0; i<(to-from); i++ ){
        const id = from + i;
        const color = colors[id];
        const originElement = document.getElementById(`sleep-point${id}_origin`);
        let alertId = 0;
        const data = day.times[i];
        const timeTable = [...Array(24)].map((_, i) => [...Array(6)].map(() => new TimeTable()));
        let total = 0;
        data.forEach( (time) => {
            const hour = time[0];
            const minute = Math.floor(time[1]/10);
            timeTable[hour][minute].n += 1;
            if ( time[2] > 0 ) timeTable[hour][minute].s += 1;
            if ( time[2] > 1 ) timeTable[hour][minute].a += 1;
        });

        // サンプルデータの挿入
        if ( is_debug ){
            for( let j=0; j<24; j++ ){
                timeTable[j] = sampleData[id][j];
            }
        }

        let linearGradient = `linear-gradient(${gradientDeg}`;
        let preTrue = true;
        let opacity = "ff";
        for ( let hour=0; hour<24; hour++ ){
            for ( let minute=0; minute<6; minute++ ){
                const p = parcent.p(hour, minute);
                const now = timeTable[hour][minute];
                const preOpacity = opacity;
                if ( now.n >= 3 ){
                    opacity = (now.s/now.n > 0.5)? "a8":"ff";
                    total += 10;
                    if ( preTrue ){
                        linearGradient +=`, ${color}${preOpacity} ${p}%`;
                        linearGradient +=`, ${color}${opacity} ${p}%`;
                    }else{
                        linearGradient +=`, #ffffff00 ${p}%`;
                        linearGradient +=`, ${color}${opacity} ${p}%`;
                        preTrue = true;
                    }
                    if( now.a > 0 ){
                        const clone_element = originElement.cloneNode(true);
                        console.log(clone_element);
                        const idname = `sleep-point${id}_${alertId++}`;
                        clone_element.id = idname;
                        clone_element.style.display = "flex";
                        clone_element.style[pointAxis] = `calc(${p}% - 6px)`;
                        originElement.after(clone_element);
                    }
                }else {
                    if ( preTrue ){
                        linearGradient += `, ${color}${preOpacity} ${p}%`;
                        linearGradient += `, #ffffff00 ${p}%`;
                        preTrue = false;
                    }
                }
            }
        }
        document.getElementById("line-box"+id).style.background = linearGradient + ")";
        document.getElementById("total"+id).innerText = `${Math.floor(total/60)}:${(total%60)||"00"}`;

    }
}

const readPayload = (data, day) => {
    const days = data[day];
    const {payload, date} = days;
    const times = [[], [], []];
    let di = 0;
    let dstr = `${date[di][0]}-${date[di][1]}-${date[di][2]}`;
    let pi = 0;
    while( pi < payload.length ){
        const {generationTime, payloadType, chNumber} = payload[pi];
        //console.log(sendDateTime, payloadType);
        if ( generationTime == null ){
            pi += 1;
        }else if( generationTime.startsWith(dstr) ){
            if( payloadType === "トリガー検出情報" ){
                const nowTime = [+generationTime.slice(11,13), +generationTime.slice(14,16), +(chNumber??0)];
                if ( times[di].slice(-1)[1] != nowTime[1] || times[di].slice(-1)[0] != nowTime[0] ){
                    times[di].push(nowTime);
                }
            }
            pi += 1;
        }else{
            di += 1;
            if( di >= times.length ) break;
            dstr = `${date[di][0]}-${date[di][1]}-${date[di][2]}`;
        }
    }
    return {date:date, times:times};
}

const main = async () => {
    const {success, data} = await getPayload();
    if (!success) {
        // 失敗時
        document.getElementById("failed").style.display = "flex";
    }else{
        // 成功時
        if( LOAD_DAYS == 3 ){
            // 3日のみ表示
            const day03 = {date, times} = readPayload(data, "day03");
            makeTimeber(day03, 0, 3);
            day03.date.forEach((day, i) => {
                document.getElementById("days"+i).innerText = `${day[1]}/${day[2]}`;
            });
        }else {
            // 7日表示
            const day03 = {date, times} = readPayload(data, "day03");
            makeTimeber(day03, 0, 3);
            const day35 = {date, times} = readPayload(data, "day35");
            makeTimeber(day35, 3, 5);
            const day57 = {date, times} = readPayload(data, "day57");
            makeTimeber(day57, 5, 7);
            [...day03.date, ...day35.date, ...day57.date].forEach((day, i) => {
                document.getElementById("days"+i).innerText = `${day[1]}/${day[2]}`;
            });
        }
    }
    loadingFeedOut();
}

main();