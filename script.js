// bolt api key and device name
var api_key = "";
var d_name = "";

// telegram api key and channel name
var botKey = "";
var c_name = "";

var interval = null;
var time = 0;

function startTimer() {
	digitalWrite(0,LOW);
    if (!interval) {
        blockUserInput();
        time = parseInt(document.getElementById("productivity_timer_seconds").value);
        if (time - Math.floor(time) !== 0) {
            alert("Invalid Input!");
            return;
        }
        boltStartTimer();
        interval = setInterval(updateTimer, 1000);
    }
}

function stopTimer() {
	digitalWrite(0,HIGH);
    if (interval) {
        unblockUserInput();
		boltStopTimer();
        clearInterval(interval);
        interval = null;
    }
    document.getElementById("time").innerHTML = "Timer: Stopped";
}

function blockUserInput() {
	document.getElementById("productivity_timer_seconds").readOnly = true;
	for (var elm of document.getElementsByClassName("checkbox__parameter"))
		elm.disabled = true;
}

function unblockUserInput() {
    document.getElementById("productivity_timer_seconds").readOnly = false;
	for (var elm of document.getElementsByClassName("checkbox__parameter"))
		elm.disabled = false;
}

function boltStartTimer() {
    // Show Bolt Animation
    if (document.getElementsByClassName("checkbox__parameter")[1].checked){
		serialWrite("digitalStartAnimationTimer:"+time);
	}
	else {
		serialWrite("digitalStartTimer:"+time);
	}
	time++; //slow down in-page timer to match delay
}

function boltStopTimer() {
	serialWrite("digitalStopTimer");
}

function updateTimer() {
	if (time-1 >= parseInt(document.getElementById("productivity_timer_seconds").value))
	document.getElementById("time").innerHTML = "Timer: Starting";
	else
    document.getElementById("time").innerHTML = "Timer: " + time;
    if (time <= 0) {
        clearInterval(interval);
        interval = null;
        document.getElementById("time").innerHTML = "Timer: Completed";
        endTimerActions();
    }
    time--;
}

function endTimerActions() {
    // Turn On String Lights
    if (document.getElementsByClassName("checkbox__parameter")[0].checked)
    digitalWrite(0,HIGH);
    // Send Remote Alert On Telegram
    if (document.getElementsByClassName("checkbox__parameter")[2].checked)
    sendTelegramAlert();
    // Run Custom Javascript On WebPage
    if (document.getElementsByClassName("checkbox__parameter")[3].checked)
    eval(document.getElementById("jsString").value);
	unblockUserInput();
}

function sendTelegramAlert(){
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200 && debug == 1) {
            alert(xmlhttp.responseText);
            var obj = JSON.parse(xmlhttp.responseText);
            if(obj.success=="1"){
                    alert(obj.value);
            }
        }
    };
    xmlhttp.open("GET", "https://api.telegram.org/bot"+botKey+"/sendMessage?chat_id="+c_name+"&text=Timer%20Completed",true);
    xmlhttp.send();
}
