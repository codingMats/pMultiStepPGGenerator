#!/bin/bash

# Target materials
MATERIALS=("WaterCa0" "Gammex_Cal_000" "Gammex_Cal_010" "Gammex_Cal_017" "Gammex_Cal_024" "Gammex_Cal_031" "Gammex_Cal_038" "Gammex_Cal_045")

# Test with a single material
MATERIALS=("WaterCa0") 

# 1e6 particles per run x 10 runs = 1e7 total particles
HISTORIES_PER_RUN=100000
HISTORIES_TOTAL=1000000
NUM_RUNS=$((HISTORIES_TOTAL / HISTORIES_PER_RUN))

# HISTORIES_PER_RUN=10000000
# HISTORIES_TOTAL=10000000000
# NUM_RUNS=$((HISTORIES_TOTAL / HISTORIES_PER_RUN))

# Base Paths
BASE_DIR="/omics/groups/OE0471/internal/m361e/master/qPaperSimulations"
MASTER_LOG_DIR="$BASE_DIR/log"
MASTER_SEED_FILE="$MASTER_LOG_DIR/master_seed_log.txt"


EXE_DIR="/omics/groups/OE0471/internal/m361e/master/pMultiStepPGGenerator"
# Executable paths
STEP1_EXE="$EXE_DIR/PGCreationStep1/build/simulation"
STEP2_EXE="$EXE_DIR/SimulationOfSetupStep2/build/simulation"

mkdir -p "$MASTER_LOG_DIR"
echo "=== TEST Submission Run: $(date) ===" >> "$MASTER_SEED_FILE"

for MAT in "${MATERIALS[@]}"; do
    MAT_PS_DIR="$BASE_DIR/data/materialPhaseSpaces/$MAT"
    MAT_TMP_DIR="$MAT_PS_DIR/tmp_multifiles"
    MAT_LOG_DIR="$MAT_PS_DIR/log"
    MAT_DET_OUT_DIR="$BASE_DIR/data/detectorSimulationOutput/$MAT"

    mkdir -p "$MAT_TMP_DIR"
    mkdir -p "$MAT_LOG_DIR"
    mkdir -p "$MAT_DET_OUT_DIR"

    MAT_SEED=$((1 + RANDOM % 1000000))
    echo "Material: $MAT | Base Seed: $MAT_SEED" >> "$MASTER_SEED_FILE"

    # 1. Step 1: 10 Runs of 1e6 histories (Array)
    STEP1_JOB_NAME="step1_${MAT}"
    bsub -J "${STEP1_JOB_NAME}[1-${NUM_RUNS}]" \
         -R "select[ncpus>=128 && maxmem>500000] rusage[mem=1G]" -n 1 -q medium -N \
         -o "$MAT_LOG_DIR/step1_%J_%I.out" \
         "bash -c 'RUN_NUM=\$LSB_JOBINDEX; $STEP1_EXE -mat $MAT -run \$RUN_NUM -p $HISTORIES_PER_RUN -n 1 -seed $MAT_SEED'"

    # 2. Merge Job (Waits for Step 1 Array to finish)
    # The \I typo has been removed here
    MERGE_JOB_NAME="merge_${MAT}"
    bsub -J "$MERGE_JOB_NAME" \
         -w "done(${STEP1_JOB_NAME})" \
         -R "select[ncpus>=128 && maxmem>500000] rusage[mem=32G]" -n 1 -q medium -N\
         -o "$MAT_LOG_DIR/merge_%J.out" \
         "hadd -f $MAT_PS_DIR/PhaseSpace_${MAT}.root $MAT_TMP_DIR/PhaseSpace_${MAT}_run*.root"

    # 3. Step 2 (Waits for Merge Job to finish)
    STEP2_JOB_NAME="step2_${MAT}"
    bsub -J "$STEP2_JOB_NAME" \
         -w "done(${MERGE_JOB_NAME})" \
         -R "select[ncpus>=128 && maxmem>500000] rusage[mem=32G]" -n 10 -q medium -N\
         -o "$MAT_LOG_DIR/step2_%J.out" \
         "$STEP2_EXE" -mat "$MAT" -run 1 -n 10 -seed "$MAT_SEED"
done