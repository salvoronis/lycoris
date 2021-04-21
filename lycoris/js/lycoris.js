const lycorisLib = require('./build/Release/lycorislib.node')

if (!(process.argv[2] == undefined)) {
    switch (process.argv[2]) {
        case "-l":
        case "--list":
            console.log(lycorisLib.listDevises())
            break
        case "-d":
        case "--device":
            if (!(process.argv[3] == undefined)) {
                lycorisLib.initLycoris(process.argv[3])
                if (lycorisLib.checkMagic() == 1){
                    lycorisLib.runInteractive()
                } else {
                    console.log("This fs is not reiserfs")
                }
            } else {
                console.log("Missing argument")
            }
            break
        case "-m":
        case "--meta":
            if (process.argv[3] != undefined){
                lycorisLib.initLycoris(process.argv[3])
                if (lycorisLib.checkMagic() == 1) {
                    console.log(lycorisLib.printMeta())
                } else {
                    console.log("This fs is not reiserfs")
                }
            } else {
                console.log("Missing argument")
            }
            break
        case "-h":
        case "--help":
            printHelp()
            break;
        default:
            console.log("Unknown command. Use -h or --help to get usage information")
            break
    }
} else {
    console.log("Put some flag")
}

function printHelp() {
    let help_list = [
        {required: false, shortName: 'l', longName: "list", description: "shows list of devices and partitions"},
        {required: false, shortName: 'h', longName: "help", description: "show this message"},
        {required: false, shortName: 'd', longName: "device", description: "start interactive mode with choosen device"},
        {required: false, shortName: 'm', longName: "meta", description: "prints superblock of choosen device"},
    ]

    help_list.forEach(e => {
        console.log(
            `${e.longName}:
    required: ${e.required}
    short name: ${e.shortName}
    long name: ${e.longName}
    description: ${e.description}`
            )
    });
}