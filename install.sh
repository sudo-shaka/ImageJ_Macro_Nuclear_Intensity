#!/bin/bash

/bin/gcc PostMacro.c -o PostMacro && /bin/sudo /bin/mv PostMacro /usr/local/bin/PostMacro && /bin/echo -e "Installed!\n" && /bin/echo -e "cd to folder containing data and type PostMacro to anaylize!\n" || /bin/echo -e "[!!] Error!"
