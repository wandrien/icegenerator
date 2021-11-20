#include <pthread.h>
#include <stdio.h>

#include "config.h"
#include "globals.h"
#include "utils.h"

#include "external_handlers.h"

pthread_mutex_t start_stop_handler_lock = PTHREAD_MUTEX_INITIALIZER; 
static FILE * start_stop_process = NULL;

static void run_start_stop_command(const std::string &command)
{
  pthread_mutex_lock(&start_stop_handler_lock);

  if (start_stop_process) {
    pclose(start_stop_process);
    start_stop_process = NULL;
  }

  if (command.size() > 0)
    start_stop_process = popen(command.c_str(), "we");

  pthread_mutex_unlock(&start_stop_handler_lock);
}

static void run_handler_on_track_start_stop(config_key_type config_key)
{
  const char * command = conf_obj->GetValue(config_key);
  if (!command || !command[0])
    return;

  std::map<char, std::string> variables{
    { 'p', ready_obj->GetData()}
  };

  std::string prepared_command = substitute_prefixed_variables(
    command,
    variables,
    '%',
    true);

  run_start_stop_command(prepared_command);
}

void run_handler_on_track_start()
{
  run_handler_on_track_start_stop(CONFIG_ON_TRACK_START);
}

void run_handler_on_track_stop()
{
  run_handler_on_track_start_stop(CONFIG_ON_TRACK_STOP);
}

void wait_for_handlers()
{
  run_start_stop_command("");
}
