#!/bin/bash

source /pkgscripts/include/pkg_util.sh

package="smi-server"
displayname="SMI-server"
version="0.0.1"
beta="yes"
os_min_ver=""
description="An server for the SMI-Bus. It allows to control sun blinds an roller shutter for the Standard-Motor-Interface. "
description_ger="Ein Server für den SMI-Bus. Hiermit könnne Rollos und Markisen gesteuert werden. Die Antriebe müssen einen Anschluss für das Standard-Motor-Interface haben."
description_de="description DE"
arch="$(pkg_get_unified_platform)"
maintainer="thefischer.net"
maintainer_url="http://smiwiki.thefischer.net/"
distributor="Ingo Fischer"
distributor_url=""
adminport="8088"
dsmuidir="ui"
dsmappname="net.thefischer.smi-server"
checkport="yes"
support_center="no"
install_dep_packages=""
silent_install="no"
silent_upgrade="no"
thirdparty="yes"

[ "$(caller)" != "0 NULL" ] && return 0
pkg_dump_info
