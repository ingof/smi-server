#!/bin/bash

source /pkgscripts/include/pkg_util.sh

package="smi-server"
version="0.1.0-$(cat /source/smi-server/build)"
beta="yes"
os_min_ver=""
description="An server for the SMI-Bus. It allows to control sun blinds and roller shutter for the Standard-Motor-Interface. "
description_enu="An server for the SMI-Bus. It allows to control sun blinds and roller shutter for the Standard-Motor-Interface. "
description_ger="Ein Server für den SMI-Bus. Hiermit können Rollos und Markisen mit SMI-Interface gesteuert werden."
displayname="SMI-Server"
displayname_enu="SMI-Server"
displayname_ger="SMI-Server"
arch="$(pkg_get_unified_platform)"
maintainer="thefischer.net"
maintainer_url="https://smiwiki.thefischer.net/"
distributor="Ingo Fischer"
distributor_url=""
adminurl="/SmiControl"
adminport="8088"
dsmuidir=""
dsmappname="SYNO.SDS.SMI.smi-server"
checkport="no"
support_center="no"
install_dep_packages=""
silent_install="no"
silent_upgrade="no"
thirdparty="yes"

[ "$(caller)" != "0 NULL" ] && return 0
pkg_dump_info
