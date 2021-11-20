modules="
  circular.cpp
  config.cpp
  data_server.cpp
  data_streamer.cpp
  external_handlers.cpp
  log.cpp
  main.cpp
  meta_parser.cpp
  player.cpp
  selector.cpp
  sgnl_handler.cpp
  streamer.cpp
  synch.cpp
  utils.cpp
"


for module in $modules ; do
  ./gen_catch_stub.sh $module > _catch_stub_${module}.tmp
  if cmp --silent _catch_stub_${module}.tmp _catch_stub_${module} ; then
    rm _catch_stub_${module}.tmp
  else
    mv _catch_stub_${module}.tmp _catch_stub_${module}
  fi
done
