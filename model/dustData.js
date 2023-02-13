const mongoose = require('mongoose')
const Schema = mongoose.Schema;
const ObjectId = Schema.ObjectId;

const dustDataSchema = new Schema({
  _id   : ObjectId,
  node  : Number,
  value : String,
  date  : { type: Date, default : () => Date.now() + 7*60*60*1000, index: true }
});

const dustData = mongoose.model('dustData', dustDataSchema)

module.exports = dustData;

// console.log(Date.now);