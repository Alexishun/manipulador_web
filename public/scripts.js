function sendData() {
    var angQ1 = document.getElementById('mov1').value;
    var angQ2 = document.getElementById('mov2').value;
    var angQ3 = document.getElementById('mov3').value;
    var angQ4 = document.getElementById('mov4').value;
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/api/update', true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    xhr.send('angQ1=' + angQ1 + '&angQ2=' + angQ2 + '&angQ3=' + angQ3 + '&angQ4=' + angQ4);
    xhr.onload = function () {
      if (xhr.status == 200) {
        console.log('Valores enviados: ' + xhr.responseText);
      }
    }
  }
  
  function updateAlmacen(almacen) {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/routine', true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    xhr.send('almacen=' + almacen);
    xhr.onload = function () {
      if (xhr.status == 200) {
        console.log('Rutina ejecutada: ' + xhr.responseText);
        var data = JSON.parse(xhr.responseText);
        document.getElementById('almacen-a').value = data.cantA;
        document.getElementById('almacen-b').value = data.cantB;
      }
    }
  }
  
  function captureImage(resolution) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', resolution, true);
    xhr.send();
    xhr.onload = function () {
      if (xhr.status == 200) {
        console.log('Image captured: ' + xhr.responseText);
      }
    }
  }
  