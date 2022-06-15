const char webpage [] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
</head>
<style>
</style>
<script>
function upDateData()
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function()
  {
    if(this.readyState == 4 && this.status == 200) // tin hieu do con esp gui ve neu co thi = 4 va = 200
    {
      var dataVDK = xhttp.responseText; // du lieu tổng do con esp gửi về nằm trong biến này
      console.log("Dữ liệu vdk:" + dataVDK); // hien thi ra
      var DataJson = JSON.parse(dataVDK);
      
      
      if(DataJson.ND != null)
      {
        document.getElementById("nhietdo").innerHTML = DataJson.ND;
      }
      
      if(DataJson.DA != null)
      {
        document.getElementById("doam").innerHTML = DataJson.DA;
      }
      
      if(DataJson.TB1 != null)
      {
        if (DataJson.TB1 ==1)
        {
          document.getElementById("tttb1").innerHTML = "On";
        }
         
        else if(DataJson.TB1 ==0)
        {
          document.getElementById("tttb1").innerHTML = "Off";
        }
      
      }
      
      if(DataJson.TB2 != null)
      {
        if (DataJson.TB2 ==1)
        {
          document.getElementById("tttb2").innerHTML = "On";
        }
         
        else if(DataJson.TB2 ==0)
        {
          document.getElementById("tttb2").innerHTML = "Off";
        }
      
      }
    
    }
  }
  xhttp.open('GET','/Update',true);
  xhttp.send();
  
  setTimeout(function() { upDateData()},1000); //goi lai ham,cu 1s goi lai mot lan. Yeu cau con esp tra tin hieu ve
  
}

function getButtonData(data)
{
  switch (data)
  {
    case 1:
      console.log("Button thiet bi 1 onclick " );
      var thietBi1=document.getElementById("tttb1").innerHTML ;
      console.log(thietBi1 );
      if (thietBi1 == "On")
      {
      var tx = "AOB"
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET","/Button?dataOfButton="+tx,true);
      xhttp.send();
      
      }
      else if(thietBi1=="Off")
      {
      var tx = "A1B"
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET","/Button?dataOfButton="+tx,true);
      xhttp.send();
      }
      break;
    case 2:
      console.log("Button thiet bi 2 onclick " );
      var thietBi2=document.getElementById("tttb2").innerHTML ;
      console.log(thietBi2 );
      if (thietBi2 == "On")
      {
      var tx = "COD"
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET","/Button?dataOfButton="+tx,true);
      xhttp.send();
      
      }
      else if(thietBi2=="Off")
      {
      var tx = "C1D"
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET","/Button?dataOfButton="+tx,true);
      xhttp.send();
      }
      break;
  }
}

function getInPut(data)
{
  switch (data)
  {
    case 1:
      var c1="E" + document.getElementById("cai1").value + "F" ;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET","/Input?_input="+c1,true);
      xhttp.send();
      break;
    case 2:
      var c2="G" + document.getElementById("cai2").value + "H" ;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET","/Input?_input="+c2,true);
      xhttp.send();
      break;
  }

} 
</script>
<body onload="upDateData()">

</body>


<label> Hi </label>
<br>
<br>
<button onclick ="getButtonData(1)"> Thiet Bi 1 </button>
<br>
<br>
<button onclick ="getButtonData(2)"> Thiet Bi 2 </button>
<br>
<br>
<input id="cai1">
<button onclick = getInPut(1)>Send 1 </button>
<br>
<br>
<input id="cai2">
<button onclick = getInPut(2)>Send 2 </button>


<h1>
Nhiet Do:
<label id = "nhietdo">0</label>
</h1>

<h1>
Do Am:
<label id = "doam">0</label>
</h1>

<h1>
Trang thai thiet bi 1:
<label id = "tttb1"> Off </label>
</h1>

<h1>
Trang thai thiet bi 2:
<label id = "tttb2"> Off </label>
</h1>



</html>





)=====";
