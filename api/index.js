const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const port = process.env.PORT || 3000;

let sensorData = { angQ1: 0, angQ2: 0, angQ3: 0, angQ4: 0 };

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

app.post('/api/update', (req, res) => {
  sensorData.angQ1 = req.body.angQ1;
  sensorData.angQ2 = req.body.angQ2;
  sensorData.angQ3 = req.body.angQ3;
  sensorData.angQ4 = req.body.angQ4;
  console.log('Data received:', sensorData);
  res.send('Data received');
});

app.get('/api/sensordata', (req, res) => {
  res.json(sensorData);
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
