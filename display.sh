#!/bin/bash

# display.sh

# Check environment

if [[ -z ${ROOTSYS} ]]; then
  echo "display.sh :: ERROR: environment is not set, please set ROOT."
  exit 1
fi

# Parse input

input=${1}

if [[ ${#input} -ne 16 ]]; then
  echo "display.sh :: ERROR: The input should contain exactly 16 digits."
  exit 1
fi

run_number=${input:0:6}
dir_number="00${input:6:1}"
event=${input:7:3}
idx_v0a=${input:10:3}
idx_v0b=${input:13:3}

# convert into integers
event=$(expr "${event}" + 0)
idx_v0a=$(expr "${idx_v0a}" + 0)
idx_v0b=$(expr "${idx_v0b}" + 0)

echo "display.sh :: Initiating display.sh"
echo "display.sh :: ====================="
echo "display.sh :: >> Run Number = ${run_number}"
echo "display.sh :: >> Dir Number = ${dir_number}"
echo "display.sh :: >> Event      = ${event}"
echo "display.sh :: >> Index V0A  = ${idx_v0a}"
echo "display.sh :: >> Index V0B  = ${idx_v0b}"
echo "display.sh ::"

# Get files

file_path="/misc/alidata121/alice_u/borquez/analysis/output/signal+bkg/${run_number}/SexaquarkResults_CustomV0s_${dir_number}.root"
file_name="SexaquarkResults_CustomV0s_${run_number}_${dir_number}.root"
echo "display.sh :: Downloading ${file_path} from alice-serv14"

echo -n "display.sh :: "; scp alice-serv14.physi.uni-heidelberg.de:${file_path} ${file_name}
echo "display.sh :: "

# Execute command

echo "display.sh :: Starting ROOT"
echo -n "display.sh :: "; root 'SexaquarkDisplay.C("'${file_name}'", '${event}', '${idx_v0a}',  '${idx_v0b}')' &> ${input}.log
