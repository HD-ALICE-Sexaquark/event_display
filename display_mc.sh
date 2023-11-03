#!/bin/bash

# display_mc.sh

# Check environment

if [[ -z ${ROOTSYS} ]]; then
  echo "display_mc.sh :: ERROR: environment is not set, please set ROOT."
  exit 1
fi

# Parse input

input=${1}

if [[ ${#input} -ne 16 ]]; then
  echo "display_mc.sh :: ERROR: The input should contain exactly 16 digits."
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


echo "display_mc.sh :: Initiating display_mc.sh"
echo "display_mc.sh :: ====================="
echo "display_mc.sh :: >> Run Number = ${run_number}"
echo "display_mc.sh :: >> Dir Number = ${dir_number}"
echo "display_mc.sh :: >> Event      = ${event}"
echo "display_mc.sh :: >> Index V0A  = ${idx_v0a}"
echo "display_mc.sh :: >> Index V0B  = ${idx_v0b}"
echo "display_mc.sh ::"

# Get files

file_path="/misc/alidata121/alice_u/borquez/analysis/output/signal+bkg/${run_number}/AnalysisResults_CustomV0s_${dir_number}.root"
file_name="AnalysisResults_CustomV0s_${run_number}_${dir_number}.root"
echo "display_mc.sh :: Downloading ${file_path} from alice-serv14"

echo -n "display_mc.sh :: "; scp alice-serv14.physi.uni-heidelberg.de:${file_path} ${file_name}
echo "display_mc.sh :: "

# Execute command

echo "display_mc.sh :: Starting ROOT"
echo -n "display_mc.sh :: "; root 'EventDisplay.C("./'${file_name}'", '${event}', '${idx_v0a}', '${idx_v0b}')' &> ${input}.log
