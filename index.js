const mongoose = require('mongoose')
const config = require('config');
const dbConfig = config.get('dustData.dbConfig.dbName');
const mqtt = require("mqtt")
var client = mqtt.connect('mqtt://broker.hivemq.com')
const fs = require('fs')

const topic = config.get('dustData.dbConfig.topic');

const dustData = require('./model/dustData')  

mongoose.set('strictQuery', true);

const connectDB = async () => {
  try {
    await mongoose.connect(
      dbConfig,
      { useNewUrlParser: true, useUnifiedTopology: true }
    )
    console.log('Connected to mongoDB')
  } catch (error) {
    console.log(error)
    process.exit(1)
  }
}

connectDB()

client.on("connect", function(){
  client.subscribe(topic)
  console.log("Connect to broker successfully !")
});

client.on('message', function (topic, message) {
  // message is Buffer
  var str = message.toString();
  // console.log(str)
  var res = str.split(";");
  // console.log(res[0])
  // console.log(res[1])

  const dustDt = new dustData({
    _id               : mongoose.Types.ObjectId(),
    node              : res[0],
    concentration     : res[1],
  }); 

  dustDt.save();

  str = message.toString() + "\n";
  console.log(message.toString())
  fs.appendFile('log.txt', str, err => {
      if (err) {
        console.error(err);
      }
      // done!
  });
})

// const dustDt = new dustData({
//   _id   : mongoose.Types.ObjectId(),
//   node  : 'Node1',
//   value : '192',
// });


// dustDt.save();
