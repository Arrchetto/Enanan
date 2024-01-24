const char html_page[] PROGMEM = R"raw(
<!DOCTYPE html>
<html>
  <style>
    body {font-family: sans-serif;}
    h1 {text-align: center; font-size: 30px;}
    p.f1 {text-align: center; color: #000000; font-size: 24px;} /* F1 Color */
    p.f2 {text-align: center; color: #000000; font-size: 24px;} /* F2 Color */
    p.f3 {text-align: center; color: #000000; font-size: 24px;} /* F3 Color */
    p.f4 {text-align: center; color: #000000; font-size: 24px;} /* F4 Color */
    p.f5 {text-align: center; color: #000000; font-size: 24px;} /* F5 Color */
    p.f6 {text-align: center; color: #000000; font-size: 24px;} /* F6 Color */
    p.f7 {text-align: center; color: #000000; font-size: 24px;} /* F7 Color */
    p.f8 {text-align: center; color: #000000; font-size: 24px;} /* F8 Color */
    p.clear {text-align: center; color: #000000; font-size: 24px;} /* Clear Color */
    p.nir {text-align: center; color: #000000; font-size: 24px;} /* NIR Color */
    p.temperature {text-align: center; color: #000000; font-size: 24px;} /* Temperature */
    span.color-block {display: inline-block; width: 20px; height: 20px; margin-left: 5px;} /* Color block style */
    p.small {font-size: 12px; color: #000000;} /* Small font for wavelength comments */
    .button-container {text-align: center; margin-top: 20px;}
    button, input {margin: 5px;}
    .center-text {text-align: center;}
  </style>

<body>
  <h1>AS7341 with ESP32 </h1><br>
  <p class="temperature">Temperature = <span id="_Temperature">0</span><span class="small">(Temperature value)</span></p>
  <p class="f1">F1 = <span id="_F1">0</span><span class="color-block" style="background-color: #0080FF;"></span><span class="small">(405-425nm)</span></p>
  <p class="f2">F2 = <span id="_F2">0</span><span class="color-block" style="background-color: #00FFFF;"></span><span class="small">(435-455nm)</span></p>
  <p class="f3">F3 = <span id="_F3">0</span><span class="color-block" style="background-color: #00FF80;"></span><span class="small">(470-490nm)</span></p>
  <p class="f4">F4 = <span id="_F4">0</span><span class="color-block" style="background-color: #00FF00;"></span><span class="small">(505-525nm)</span></p>
  <p class="f5">F5 = <span id="_F5">0</span><span class="color-block" style="background-color: #80FF00;"></span><span class="small">(545-565nm)</span></p>
  <p class="f6">F6 = <span id="_F6">0</span><span class="color-block" style="background-color: #FFFF00;"></span><span class="small">(580-600nm)</span></p>
  <p class="f7">F7 = <span id="_F7">0</span><span class="color-block" style="background-color: #FF8000;"></span><span class="small">(620-640nm)</span></p>
  <p class="f8">F8 = <span id="_F8">0</span><span class="color-block" style="background-color: #FF0000;"></span><span class="small">(670-690nm)</span></p>
  <p class="clear">Clear = <span id="_Clear">0</span></span><span class="small">(Clear wavelength range)</span></p>
  <p class="nir">NIR = <span id="_NIR">0</span></span><span class="small">(NIR wavelength range)</span></p>

  <!-- Display variables R, G, B -->
  <p class="center-text">R = <span id="_R">0</span></p>
  <p class="center-text">G = <span id="_G">0</span></p>
  <p class="center-text">B = <span id="_B">0</span></p>

  <!-- New buttons for brightness adjustment -->
  <div class="button-container">
    <button onclick="sendBrightnessRequest('uplight')">OPEN</button>
    <button onclick="sendBrightnessRequest('downlight')">CLOSE</button>
  </div>

  <!-- New text input fields and send button -->
  <div class="button-container">
    <input type="text" id="input1" placeholder="R">
    <input type="text" id="input2" placeholder="G">
    <input type="text" id="input3" placeholder="B">
    <input type="text" id="input4" placeholder="LIGHT">
    <button onclick="sendInputs()">Send</button>
  </div>

  <script>
    // Brightness adjustment function
    function sendBrightnessRequest(action) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", action, true);
      xhttp.send();
    }

    // Send inputs function
    function sendInputs() {
      var input1 = document.getElementById("input1").value;
      var input2 = document.getElementById("input2").value;
      var input3 = document.getElementById("input3").value;
      var input4 = document.getElementById("input4").value;

      // 创建一个 XMLHttpRequest 对象
      var xhttp = new XMLHttpRequest();

      // 定义请求的类型、URL 以及是否异步
      xhttp.open("GET", "/sendInputs?input1=" + input1 + "&input2=" + input2 + "&input3=" + input3 + "&input4=" + input4, true);

      // 发送请求
      xhttp.send();

      // 打印输入值到控制台
      console.log("Input 1: " + input1);
      console.log("Input 2: " + input2);
      console.log("Input 3: " + input3);
      console.log("Input 4: " + input4);
    }

    setInterval(function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          const text = this.responseText;
          const myArr = JSON.parse(text);
          document.getElementById("_F1").innerHTML = myArr[0];
          document.getElementById("_F2").innerHTML = myArr[1];
          document.getElementById("_F3").innerHTML = myArr[2];
          document.getElementById("_F4").innerHTML = myArr[3];
          document.getElementById("_F5").innerHTML = myArr[4];
          document.getElementById("_F6").innerHTML = myArr[5];
          document.getElementById("_F7").innerHTML = myArr[6];
          document.getElementById("_F8").innerHTML = myArr[7];
          document.getElementById("_Clear").innerHTML = myArr[8];
          document.getElementById("_NIR").innerHTML = myArr[9];
          document.getElementById("_Temperature").innerHTML = myArr[10];
          document.getElementById("_R").innerHTML = myArr[11]; // Display R
          document.getElementById("_G").innerHTML = myArr[12]; // Display G
          document.getElementById("_B").innerHTML = myArr[13]; // Display B
        }
      };
      xhttp.open("GET", "readColors", true);
      xhttp.send();
    }, 50);  // Updated to 50 milliseconds
  </script>
</body>
</html>
)raw";
