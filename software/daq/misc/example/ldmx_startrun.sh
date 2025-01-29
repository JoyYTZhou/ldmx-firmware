#!/usr/bin/env sh

which=$1


# --------------------------------------------
# If no user parameter, default to the example
# --------------------------------------------
if [ -z $which ]
then
    which="example"
fi    

# -------------------------------------------------------------------
# Construct the path to EUDAQ module library and check that it exists
# -------------------------------------------------------------------
eudaq_module_dir=$LDMX/install/x86_64-linux/lib/eudaq/$which
if [ ! -d ${eudaq_module_dir} ]
then
    echo "Error: ${eudaq_module_dir} , EUDAQ module library does not exist"
    exit -1
fi

echo "LDMX eudaq being activator: ${which}"

BINPATH=${EUDAQ}/bin
export EUDAQ_MODULE_DIR=${eudaq_module_dir}
save=${EUDAQ_MODULE_DIR}



# -----------------
# Start Run Control
# -----------------
${BINPATH}/euRun -n LdmxRunControl &
sleep 2


# ------------
# Start Logger
# ------------
unset EUDAQ_MODULE_IGNORE_DEFALUT
unset EUDAQ_MODULE_DIR
${BINPATH}/euLog &
sleep 2


export EUDAQ_MODULE_IGNORE_DEFALUT=YES
export EUDAQ_MODULE_DIR=${save}

# -------------------
# Start Event Monitor
# -------------------
${BINPATH}/euCliMonitor -n LdmxMonitor -t LdmxMon &


# --------------------
# Start Data Collector
# --------------------
${BINPATH}/euCliCollector -n LdmxTgDataCollector -t LdmxDc &


# The following data collectors are provided if you build user/eudet
#$BINPATH/euCliCollector -n DirectSaveDataCollector -t my_dc &
#$BINPATH/euCliCollector -n EventIDSyncDataCollector -t my_dc &
#$BINPATH/euCliCollector -n TriggerIDSyncDataCollector -t my_dc &


# ------------------------
# Start the Data Producers
# ------------------------
${BINPATH}/euCliProducer -n LdmxProducer -t Ldmx0 &
${BINPATH}/euCliProducer -n LdmxProducer -t Ldmx1 &
