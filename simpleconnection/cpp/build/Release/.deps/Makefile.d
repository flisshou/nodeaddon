cmd_Makefile := cd ..; /opt/local/lib/node_modules/node-gyp/gyp/gyp_main.py -fmake --ignore-environment "--toplevel-dir=." -I/Users/flisshou/Desktop/nodeaddon/simpleconnection/cpp/build/config.gypi -I/opt/local/lib/node_modules/node-gyp/addon.gypi -I/Users/flisshou/.node-gyp/4.4.3/include/node/common.gypi "--depth=." "-Goutput_dir=." "--generator-output=build" "-Dlibrary=shared_library" "-Dvisibility=default" "-Dnode_root_dir=/Users/flisshou/.node-gyp/4.4.3" "-Dnode_gyp_dir=/opt/local/lib/node_modules/node-gyp" "-Dnode_lib_file=node.lib" "-Dmodule_root_dir=/Users/flisshou/Desktop/nodeaddon/simpleconnection/cpp" binding.gyp