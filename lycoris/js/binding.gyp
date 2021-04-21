{
	"targets": [
		{
			"target_name": "lycorislib",
			"sources": [
                "cppbindings/bindings.cpp",
                "../core/src/btree.c",
                "../core/src/directory.c",
                "../core/src/file.c",
                "../core/src/linked_list.c",
                "../core/src/list.c",
                "../core/src/reiser.c",
                "../core/src/util.c"
                ],
			'include_dirs': [
        			"<!@(node -p \"require('node-addon-api').include\")"
      			],
			'dependencies': [
          			"<!(node -p \"require('node-addon-api').gyp\")"
      			],
			'cflags!': [ '-fno-exceptions' ],
      		'cflags_cc!': [ '-fno-exceptions' ]
		}
	]
}