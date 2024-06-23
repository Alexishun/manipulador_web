// Obtener referencia a la base de datos
var database = firebase.database();

function sendData() {
  var angQ1 = document.getElementById('mov1').value;
  var angQ2 = document.getElementById('mov2').value;
  var angQ3 = document.getElementById('mov3').value;
  var angQ4 = document.getElementById('mov4').value;

  // Enviar datos a Firebase
  database.ref('sensorData').set({
    angQ1: angQ1,
    angQ2: angQ2,
    angQ3: angQ3,
    angQ4: angQ4
  }).then(function() {
    console.log('Valores enviados');
  }).catch(function(error) {
    console.log('Error en el envío: ' + error);
  });
}

function updateAlmacen(almacen) {
  // Actualizar datos en Firebase
  database.ref('almacen').set({
    almacen: almacen
  }).then(function() {
    console.log('Rutina ejecutada');
  }).catch(function(error) {
    console.log('Error en la actualización: ' + error);
  });

  // Simulación de actualización de cantidades
  var data = { cantA: Math.floor(Math.random() * 100), cantB: Math.floor(Math.random() * 100) };
  document.getElementById('almacen-a').value = data.cantA;
  document.getElementById('almacen-b').value = data.cantB;
}

function captureImage(resolution) {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', resolution, true);
  xhr.send();
  xhr.onload = function () {
    if (xhr.status == 200) {
      console.log('Image captured: ' + xhr.responseText);
      // Si necesitas guardar datos de la imagen en Firebase, agrega el código aquí.
    }
  }
}
