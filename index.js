const mongoose = require('mongoose')
const config = require('config');
const dbConfig = config.get('dustData.dbConfig.dbName');
const mqtt = require("mqtt")
var client = mqtt.connect('mqtt://broker.hivemq.com')
const fs = require('fs')
const Schema = mongoose.Schema;
const ObjectId = Schema.ObjectId;

const topic = config.get('dustData.dbConfig.topic');

// const dustData = require('./model/dustData')  
// const dustData1 = require('./model/dustData1')  

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


const Data3DSchema = new Schema({
  node          : Number,
  concentration : Number,
  date          : { type: Date, default : () => Date.now() + 7*60*60*1000, index: true }
});

const Data3D = mongoose.model('Data3D', Data3DSchema);

const DataOpenSchema = new Schema({
  node          : Number,
  concentration : Number,
  date          : { type: Date, default : () => Date.now() + 7*60*60*1000, index: true }
});

const DataOpen = mongoose.model('DataOpenWide', DataOpenSchema);

client.on('message', function (topic, message) {
  // message is Buffer
  var str = message.toString();
  // console.log(str)
  var res = str.split(";");
  // console.log(res[0])
  // console.log(res[1])

  const dustDt = new DataOpen({
    // _id               : mongoose.Types.ObjectId(),
    node              : res[0],
    concentration     : res[1],
  }); 

  dustDt.save();

  str = message.toString() + "\n";
  console.log(message.toString())
  fs.appendFile('log1.txt', str, err => {
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
