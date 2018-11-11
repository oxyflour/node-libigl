const mod = require('bindings')('binding')
console.time('hello')
mod.hello()
console.timeEnd('hello')
