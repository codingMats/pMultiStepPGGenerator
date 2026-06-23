#!/bin/bash

# Target materials
# MATERIALS=("WaterCa0" "Gammex_Cal_000" "Gammex_Cal_010" "Gammex_Cal_017" "Gammex_Cal_024" "Gammex_Cal_031" "Gammex_Cal_038" "Gammex_Cal_045")

# Test with a single material
MATERIALS=("WaterCa0") 

# 1e6 particles per run x 10 runs = 1e7 total particles
HISTORIES_PER_RUN=100000
HISTORIES_TOTAL=1000000
NUM_RUNS=$((HISTORIES_TOTAL / HISTORIES_PER_RUN))

# Local Base Paths
BASE_DIR="/home/mats/projects/geant4/pMultiStepPGGenerator"
DATA_DIR="$BASE_DIR/data"
MASTER_LOG_DIR="$BASE_DIR/log"
MASTER_SEED_FILE="$MASTER_LOG_DIR/master_seed_log.txt"

# Local Executable paths
STEP1_EXE="$BASE_DIR/PGCreationStep1/build/simulation"
# Assuming Step 2 executable follows the original directory structure pattern
STEP2_EXE="$BASE_DIR/SimulationOfSetupStep2/build/simulation"

mkdir -p "$MASTER_LOG_DIR"
echo "=== LOCAL TEST Submission Run: $(date) ===" >> "$MASTER_SEED_FILE"

for MAT in "${MATERIALS[@]}"; do
    MAT_PS_DIR="$DATA_DIR/materialPhaseSpaces/$MAT"
    MAT_TMP_DIR="$MAT_PS_DIR/tmp_multifiles"
    MAT_LOG_DIR="$MAT_PS_DIR/log"
    MAT_DET_OUT_DIR="$DATA_DIR/detectorSimulationOutput/$MAT"

    mkdir -p "$MAT_TMP_DIR"
    mkdir -p "$MAT_LOG_DIR"
    mkdir -p "$MAT_DET_OUT_DIR"

    MAT_SEED=$((1 + RANDOM % 1000000))
    echo "Material: $MAT | Base Seed: $MAT_SEED" >> "$MASTER_SEED_FILE"

    echo "Starting Step 1 for $MAT ($NUM_RUNS runs)..."
    
    # 1. Step 1: Local Loop
    # Note: This is written sequentially to prevent overloading a local machine.
    # See the critical evaluation below regarding parallel execution.
    for RUN_NUM in $(seq 1 $NUM_RUNS); do
        echo "  Running Step 1, Run $RUN_NUM..."
        "$STEP1_EXE" -mat "$MAT" -run "$RUN_NUM" -p "$HISTORIES_PER_RUN" -n 1 -seed "$MAT_SEED" > "$MAT_LOG_DIR/step1_${RUN_NUM}.out" 2>&1
    done

    echo "Starting Merge for $MAT..."
    
    # 2. Merge Job
    hadd -f "$MAT_PS_DIR/PhaseSpace_${MAT}.root" "$MAT_TMP_DIR/PhaseSpace_${MAT}_run"*.root > "$MAT_LOG_DIR/merge.out" 2>&1

    echo "Starting Step 2 for $MAT..."
    
    # 3. Step 2
    "$STEP2_EXE" -mat "$MAT" -run 1 -n 10 -seed "$MAT_SEED" > "$MAT_LOG_DIR/step2.out" 2>&1

    echo "Finished processing $MAT."
done