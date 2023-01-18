  #!/usr/bin/env bash

PROGNAME=$0
PROGDIR=$(dirname ${PROGNAME})
CACHE_FILE="${PROGDIR}/.cache"

DEFAULT_DIR=${PROGDIR}/..

DESTROY=
LOCKCOMM=
PROVISION=
STOP=
COMMAND=
NEWSDK=

TOOLS="vagrant grep cut"
MISSING_TOOLS=""

VERBOSE=0
SCRAPE_SNAPSHOTS=0

realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

ucase() {
  if [ "`which tr`" != "" ]; then
    echo ${1} | tr '[a-z]' '[A-Z]'
  else
    echo ${1^^}
  fi
}

for TOOL in ${TOOLS}; do
  WHICH_TOOL=$(which ${TOOL})
  eval $(ucase ${TOOL})=${WHICH_TOOL}
  if [ "${WHICH_TOOL}" = "" ]; then
    MISSING_TOOLS="${MISSING_TOOLS} ${TOOL}"
  fi
done

if [ "${MISSING_TOOLS}" != "" ]; then
  echo "$(basename $0): Missing the following tools:" >&2
  echo ${MISSING_TOOLS}
  exit 1
fi

function usage() {
    echo "Usage: ${PROGNAME} [-h] [-d] [-p] [-s] [-m <path>] [-k <path>] [-c <command>]"
    echo -e "\t-h\t\t\tPrints this message"
    echo -e "\t-d\t\t\tDestroys the current vagrant machine"
    echo -e "\t-m <path>\t\tPath to project repository"
    echo -e "\t-c <command>\t\tExecutes command on the virtual machine"
    echo -e "\t-p\t\t\tForces provisioning"
    echo -e "\t-s\t\t\tStops the virtual machine"
}

while getopts "dhm:c:ps" opt; do
    case $opt in
	d)  DESTROY="true";;
	h)  usage && exit 0;;
	m)  DIR=$OPTARG;;
	c)  COMMAND=$OPTARG;;
	p)  PROVISION="true";;
	s)  STOP="true";;
	\?) echo -e "Invalid option\n" && usage && exit 1;;
	:)  echo -e "Option -$OPTARG requires an argument\n" && usage && exit 2;;
    esac
done

# scrape the VM ID from vagrant to avoid stomping on other VMs
# running on the same host
if [ -d ${PROGDIR}/.vagrant ]; then
  VM_ID=$(${VAGRANT} global-status | ${GREP} ${PWD} | ${CUT} -d ' ' -f 1)
fi

if [ "$DESTROY" = "true" ]; then
    rm -f $CACHE_FILE
    echo "Destroying vagrant ${VM_ID}"
    exec ${VAGRANT} destroy ${VM_ID}
fi

if [ "$STOP" = "true" ]; then
    echo "Halting vagrant ${VM_ID}"
    exec ${VAGRANT} halt ${VM_ID}
fi

# If we have a value for io store it in cache
# else read from the cache
if [ -n "$DIR" ]; then
    echo $DIR > $CACHE_FILE
elif [ -f "$CACHE_FILE" ]; then
    DIR=$( cat $CACHE_FILE )
elif [ -d ${DEFAULT_DIR} ]; then
    DIR=$(realpath ${DEFAULT_DIR})
fi

if [ -z "$DIR" ]; then
    read -p "Path to project repository not found, continue? (y/[n]) " choice
    case $choice in
	y|Y) : ;;
	*) exit 0;;
    esac
fi

if [ -z "$SSH_HOST_PORT" ]; then
  if [ -z "$EXECUTOR_NUMBER" ]; then
    SSH_HOST_PORT=2222
  else
    SSH_HOST_PORT=222${EXECUTOR_NUMBER}
  fi
fi

cd ${PROGDIR}
echo "Bringing up vagrant ${VM_ID}"
echo "PROJECT_PATH=${DIR}"
SSH_HOST_PORT=${SSH_HOST_PORT} PROJECT_PATH=${DIR} ${VAGRANT} up

if [ "$?" -ne 0 ]; then
    echo "Something failed when bringing vagrant up"
    exit 3
fi

if [ "$PROVISION" = "true" ]; then
    echo "Provisioning vagrant"
    ${VAGRANT} provision
fi

if [ "${COMMAND}" != "" ]; then
    echo "Executing ${COMMAND} on vagrant"
    exec ${VAGRANT} ssh -- -X ${COMMAND}
else
    echo "Connecting to vagrant"
    exec ${VAGRANT} ssh -- -X
fi
