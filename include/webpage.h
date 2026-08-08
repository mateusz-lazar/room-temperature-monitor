#pragma once

const char html[] = R"rawliteral(
   <html><body>Room temperature sensor<br>
    Current temperature: <span id='temp'>--</span> &deg;C<br>
    Day maximum: <span id='max'>--</span> &deg;C<br>
    Day minimum: <span id='min'>--</span> &deg;C
    
    <script>
      function data_fetch(){
        fetch('/data').then(r=>r.json()).then(d=>{
          document.getElementById('temp').innerText=d.temp;
          document.getElementById('max').innerText=d.max;
          document.getElementById('min').innerText=d.min;
        });
      }
      data_fetch();
      setInterval(data_fetch, 2000);
    </script>
  </body></html>
)rawliteral";