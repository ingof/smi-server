#!/bin/bash

source /pkgscripts/include/pkg_util.sh

package="smi-monitor"
displayname="SMI-Monitor"
version="0.0.1"
beta="yes"
os_min_ver=""
description="SMI-Monitor can monitor the SMI and SWB in hex-format."
description_ger="SMI-Monitor kann den SMI und SWB-Bus im Hex-Format mitlesen"
description_de="SMI-Monitor kann den SMI und SWB-Bus im Hex-Format mitlesen"
arch="$(pkg_get_unified_platform)"
maintainer="thefischer.net"
maintainer_url="http://smiwiki.thefischer.net/"
distributor="Ingo Fischer"
distributor_url=""
adminport="8086"
#dsmuidir="ui"
dsmappname="net.thefischer.SMI-Monitor"
checkport="yes"
support_center="no"
install_dep_packages=""
silent_install="no"
silent_upgrade="no"
thirdparty="yes"

[ "$(caller)" != "0 NULL" ] && return 0
pkg_dump_info
