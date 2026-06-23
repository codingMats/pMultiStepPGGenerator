#!/bin/bash

# Target materials
MATERIALS=("WaterCa0" "Gammex_Cal_000" "Gammex_Cal_010" "Gammex_Cal_017" "Gammex_Cal_024" "Gammex_Cal_031" "Gammex_Cal_038" "Gammex_Cal_045")

HISTORIES_PER_RUN=10000000
HISTORIES_TOTAL=10000000000
NUM_RUNS=$((HISTORIES_TOTAL / HISTORIES_PER_RUN))

# Base Paths
BASE_DIR="/home/mats/projects/geant4/pMultiStepPGGenerator"
MASTER_LOG_DIR="$BASE_DIR/log"
MASTER_SEED_FILE="$MASTER_LOG_DIR/master_seed_log.txt"

# Executable paths
STEP1_EXE="$BASE_DIR/PGCreationStep1/build/simulation"
STEP2_EXE="$BASE_DIR/SimulationOfSetupStep2/build/simulation"

# Initialize Master Log
mkdir -p "$MASTER_LOG_DIR"
echo "=== Submission Run: $(date) ===" >> "$MASTER_SEED_FILE"

for MAT in "${MATERIALS[@]}"; do
    echo "Configuring pipeline for: $MAT"

    # Define Material-Specific Paths
    MAT_PS_DIR="$BASE_DIR/data/materialPhaseSpaces/$MAT"
    MAT_TMP_DIR="$MAT_PS_DIR/tmp_multifiles"
    MAT_LOG_DIR="$MAT_PS_DIR/log"
    MAT_DET_OUT_DIR="$BASE_DIR/data/detectorSimulationOutput/$MAT"

    # Create Directories (CRITICAL: LSF fails if log directories don't exist)
    mkdir -p "$MAT_TMP_DIR"
    mkdir -p "$MAT_LOG_DIR"
    mkdir -p "$MAT_DET_OUT_DIR"

    # Generate Independent Base Seed (1 to 1,000,000)
    MAT_SEED=$((1 + RANDOM % 1000000))
    echo "Material: $MAT | Base Seed: $MAT_SEED" >> "$MASTER_SEED_FILE"

    # 1. Step 1: Phase Space Generation (Priority 10)
    STEP1_JOB_NAME="step1_${MAT}"
    bsub -J "${STEP1_JOB_NAME}[1-${NUM_RUNS}]" -sp 10 \
         -R "select[ncpus>=1] rusage[mem=1G]" -n 1 -q medium \
         -o "$MAT_LOG_DIR/step1_%J_%I.out" \
         -e "$MAT_LOG_DIR/step1_%J_%I.err" \
         "$STEP1_EXE" -mat "$MAT" -run "%I" -p "$HISTORIES_PER_RUN" -n 1 -seed "$MAT_SEED"

    # 2. Merge Job: Waits for Step 1, merges tmp files, deletes tmp files (Priority 50)
    MERGE_JOB_NAME="merge_${MAT}"
    bsub -J "$MERGE_JOB_NAME" -sp 50 \
         -w "done(${STEP1_JOB_NAME})" \
         -R "select[ncpus>=1] rusage[mem=4G]" -n 1 -q medium \
         -o "$MAT_LOG_DIR/merge_%J.out" \
         -e "$MAT_LOG_DIR/merge_%J.err" \
         "hadd -f $MAT_PS_DIR/PhaseSpace_${MAT}.root $MAT_TMP_DIR/PhaseSpace_${MAT}_run*.root && rm $MAT_TMP_DIR/PhaseSpace_${MAT}_run*.root"

    # 3. Step 2: Final Simulation (Priority 100)
    STEP2_JOB_NAME="step2_${MAT}"
    bsub -J "$STEP2_JOB_NAME" -sp 100 \
         -w "done(${MERGE_JOB_NAME})" \
         -R "select[ncpus>=10] rusage[mem=4G]" -n 10 -q medium \
         -o "$MAT_LOG_DIR/step2_%J.out" \
         -e "$MAT_LOG_DIR/step2_%J.err" \
         "$STEP2_EXE" -mat "$MAT" -run 1 -n 10 -seed "$MAT_SEED"
done

echo "All pipelines submitted. See $MASTER_SEED_FILE for seed records."