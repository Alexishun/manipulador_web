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

// Función para configurar el modo
function setControlMode(mode) {
  document.getElementById('process-status').innerText = `Modo de control: ${mode}`;
  var controlado = mode === "automatic" ? true : false;

  database.ref('Modo').set({
    controlado: controlado
  }).then(function() {
    console.log('Valores enviados');
  }).catch(function(error) {
    console.log('Error en el envío: ' +   error);
  });
}

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

function updateAlmacen(almacen) {
  var cantA = parseInt(document.getElementById('almacen-a').value);
  var cantB = parseInt(document.getElementById('almacen-b').value);

  if (almacen === "A") {
    cantA += 1;
  } else if (almacen === "B") {
    cantB += 1;
  }
  database.ref('almacen').set({
    almacen_rutina: almacen,
    cantA: cantA,
    cantB: cantB
  }).then(function() {
    console.log('Rutina ejecutada');
  }).catch(function(error) {
    console.log('Error en la actualización: ' + error);
  });
}

fileSelector.onchange = () => {
  var file = fileSelector.files[0]
  var imgUrl = window.URL.createObjectURL(new Blob([file], { type: 'image/jpg' }))
  console.log(imgUrl);
  img.src = imgUrl
}

function captureImage() {
  var file = document.getElementById('fileSelector').files[0];
  if (file) {
    var imgUrl = URL.createObjectURL(file);
    document.getElementById('captured-img').src = imgUrl;

    // Actualizar la base de datos
    database.ref('procesamiento').set({
      capturar_img: true,
      img: imgUrl,
      text_rec: ''
    }).then(function() {
      console.log('Imagen capturada y guardada en la base de datos');
      // Iniciar reconocimiento de texto
      startTextRecognition(file);
    }).catch(function(error) {
      console.log('Error al guardar la imagen: ' + error);
    });
  }
}

function startTextRecognition(file) {
  const rec = new Tesseract.TesseractWorker();
  rec.recognize(file)
    .progress(function(response) {
      if (response.status == 'recognizing text') {
        document.getElementById('progress').innerHTML = response.status + ' ' + response.progress;
      } else {
        document.getElementById('progress').innerHTML = response.status;
      }
    })
    .then(function(data) {
      document.getElementById('textarea').innerHTML = data.text;
      document.getElementById('progress').innerHTML = 'Done';

      // Actualizar la base de datos con el texto reconocido
      database.ref('procesamiento').update({
        text_rec: data.text
      }).then(function() {
        console.log('Texto reconocido guardado en la base de datos');
      }).catch(function(error) {
        console.log('Error al guardar el texto reconocido: ' + error);
      });
    });
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

database.ref('almacen').on('value', (snapshot) => {
  const data = snapshot.val();
  if (data) {
    document.getElementById('almacen-a').value = data.cantA;
    document.getElementById('almacen-b').value = data.cantB;
    console.log('Datos actualizados: ', data);
  }
});

database.ref('Modo').on('value', (snapshot) => {
  const data = snapshot.val();
  if (data) {
    var value = data.controlado;
    if (value) {
      document.getElementById('process-status').innerText = `Modo de control: Automático`;
    } else {
      document.getElementById('process-status').innerText = `Modo de control: Manual`;
    }
  }
});
