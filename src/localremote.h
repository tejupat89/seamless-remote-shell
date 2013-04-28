#pragma once

#include "command.h"

int local_or_remote(char *);
REMOTE_FILE* convert_to_remote(char *, REMOTE_FILE *); 
long calculate_size(char *);
