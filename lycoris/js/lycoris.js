const lycorisLib = require('./build/Release/lycorislib.node')
const prompt = require('prompt-sync')({sigint: true});

/*function ItemWrapWrapper(name) {
    this.greet = function(str){
        return _addonInstance.greet(str);
    }

    var _addonInstance = new lycorisLib.ItemWrapWrapper(name);
}

var item = new ItemWrapWrapper("pidor");
item.greet("huilo");*/

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
        case "-t":
        case "--test":
            test()
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

function test(){
    //console.log(lycorisLib.get_some_struct())
    //console.log(lycorisLib.changeSomeShit({
    //    dir_id:1,obj_id:2,name:"shit",type:123
    //}))
    lycorisLib.initWrapper(4)
    //lycorisLib.toItemWrapper(1,2,"jopa", 1, 0)
    for (var i = 0; i < 5; i++) {
        lycorisLib.toItemWrapper(1,2,"jopa", 1, i)
    }
}

function runInteractive(){
    var stop_flag = false;
    var cmd = "asfasd";
    var current_dir;
    var skey = {dir_id: 1, obj_id: 2};
    var inum //=get_dir();
    while (!stop_flag) {
        cmd = prompt("lycoris-> ")
        var command = cmd.split(" ")
        switch (command[0]){
            case "exit":
            case "q":
                stop_flag = true
                break
            case "pwd":
                console.log(command)
                //print_working_dir
                break
            case "ls":
                console.log(command)
                //list_directory
                break
            case "cd":
                console.log(command)
                //change_dir
                break
            case "cp":
                console.log(command)
                //copy
                break
            case "cat":
                console.log(command)
                //get_file_by_name
                break
            default:
                console.log("unknown command")
        }
    }
}