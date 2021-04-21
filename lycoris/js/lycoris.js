const lycorisLib = require('./build/Release/lycorislib.node')

lycorisLib.initLycoris("/home/salvoroni/spo/lycoris/van.fs")
console.log(lycorisLib.printMeta())
lycorisLib.runInteractive()