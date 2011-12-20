#!/bin/bash
#
#  The Open Computer Forensics Architecture moduleset.
#  Copyright (C) 2003..2008 KLPD  <ocfa@dnpa.nl>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
# Script to call photorec with command-line parameters
# Usage:
#     ocfacarv.sh <Path_to_result_dir> <unallocated_(evidence_file)> <photorec_binary_path> 
#
#

cd $1

$3 /d $1 /cmd $2 partition_i386,fileopt,everything,disable,jpg,enable,png,enable,tif,enable,gif,enable,doc,enable,pdf,enable,options,paranoid,keep_corrupted_file_no,wholespace,search > /dev/null


