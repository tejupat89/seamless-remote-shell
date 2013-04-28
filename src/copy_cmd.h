#pragma once

#include "command.h"

void copy_remote(REMOTE *, REMOTE *);
void copy_cost(COST *, COST *, int);
void copy_array(int **, int *, int);
void copy_remote_from_remote_file(REMOTE *, REMOTE_FILE *);
void copy_long_array(long **, long **, int);
