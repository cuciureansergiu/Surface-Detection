#!/bin/sh
awk '/set proj_name "/{print $NF}' project.tcl | sed 's/"$//' | sed 's/"//'