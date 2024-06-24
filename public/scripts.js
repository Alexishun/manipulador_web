// Inicializa Firebase
const firebaseConfig = {
  apiKey: "AIzaSyBdz1y5nBA0drg_ZyxwozQoaJOOz2QmWxs",
  authDomain: "manipuladorweb-c1b90.firebaseapp.com",
  databaseURL: "https://manipuladorweb-c1b90-default-rtdb.firebaseio.com",
  projectId: "manipuladorweb-c1b90",
  storageBucket: "manipuladorweb-c1b90.appspot.com",
  messagingSenderId: "941898277126",
  appId: "1:941898277126:web:2ff5281e7aa1a1c6aabe87",
  measurementId: "G-MQ5JBQKB6B"
};

firebase.initializeApp(firebaseConfig);
var database = firebase.database();

// Función para enviar datos a Firebase
function sendData() {
  var angQ1 = parseInt(document.getElementById('mov1').value);
  var angQ2 = parseInt(document.getElementById('mov2').value);
  var angQ3 = parseInt(document.getElementById('mov3').value);
  var angQ4 = parseInt(document.getElementById('mov4').value);

  // Enviar datos a Firebase como enteros
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

/// Función para actualizar datos de almacén en Firebase
function updateAlmacen(almacen) {
  // Obtener las cantidades de las cajas desde los inputs
  var cantA = parseInt(document.getElementById('almacen-a').value, 10);
  var cantB = parseInt(document.getElementById('almacen-b').value, 10);

  // Incrementar la cantidad de cajas dependiendo del almacén seleccionado
  if (almacen === "A") {
    cantA += 1;
    document.getElementById('almacen-a').value = cantA;
  } else if (almacen === "B") {
    cantB += 1;
    document.getElementById('almacen-b').value = cantB;
  }

  // Actualizar datos en Firebase
  database.ref('almacen').set({
    almacen: almacen,
    cantA: cantA,
    cantB: cantB
  }).then(function() {
    console.log('Rutina ejecutada');
  }).catch(function(error) {
    console.log('Error en la actualización: ' + error);
  });
}

// Función para capturar imagen (si es necesario)
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

// Escuchar cambios en los datos de sensorData
database.ref('sensorData').on('value', (snapshot) => {
  const data = snapshot.val();
  if (data) {
    document.getElementById('mov1').value = data.angQ1;
    document.getElementById('mov2').value = data.angQ2;
    document.getElementById('mov3').value = data.angQ3;
    document.getElementById('mov4').value = data.angQ4;
    console.log('Datos actualizados: ', data);
  }
});

// Escuchar cambios en los datos de almacen
database.ref('almacen').on('value', (snapshot) => {
  const data = snapshot.val();
  console.log('Datos de almacén actualizados: ', data);
});
