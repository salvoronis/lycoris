const lycorisLib = require('./build/Release/lycorislib.node')
const prompt = require('prompt-sync')({sigint: true});

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
                    //lycorisLib.runInteractive()
                    runInteractive()
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

function runInteractive(){
    var stop_flag = false
    var cmd = "a"
    var inum
    var current_dir = [{dir_id: 0, obj_id: 0, namee: "init"}]
    var skey = {dir_id: 1, obj_id: 2}
    var tmp = lycorisLib.getDir(skey, current_dir)
    current_dir = tmp.item_wrapper
    inum = tmp.inum
    while (!stop_flag) {
        cmd = prompt("lycoris-> ")
        var command = cmd.split(" ")
        switch (command[0]){
            case "exit":
            case "q":
                stop_flag = true
                break
            case "pwd":
                console.log(lycorisLib.printWorkingDir(skey))
                break
            case "ls":
                listDirectory(current_dir)
                break
            case "cd":
                var cdRes = lycorisLib.changeDir(current_dir, inum, command[1], skey)
                skey = cdRes.skey
                inum = cdRes.inum
                current_dir = cdRes.item_wrapper
                break
            case "cp":
                lycorisLib.copy(command[1], command[2], current_dir, inum)
                break
            case "cat":
                console.log(lycorisLib.getFileByName(command[1], current_dir, inum))
                break
            default:
                console.log("unknown command")
        }
    }
}

function listDirectory(curDir) {
    for (var i = curDir.length - 1; i >= 0; i--) {
        console.log(curDir[i].namee)
    }
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