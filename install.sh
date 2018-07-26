#!/bin/bash

doesntexist(){
    echo "$1 is not installed on the system. Please install the package that provides $1 and any needed dependencies."
    echo "It is recommended that you refer to your distro's repositories to install this before installing from any unknown or untrusted sources."
    exit 1
}

checkdepends(){
    command -v make > /dev/null || doesntexit "make"
    #just in case?
    command -v g++ > /dev/null || doesntexist "gcc"
    command -v python3 > /dev/null || doesntexist "python3"
}

installandconfigure(){

    make
    sudo make install

    mkdir -p ~/.config/custard
    cp ./examples/sxhkdrc ~/.config/custard/sxhkdrc
    cp ./examples/custard.sh ~/.config/custard/rc.sh

    cat <<EOF>> ~/.config/custard/start.sh
\#!/bin/bash
echo '' > /tmp/custard.log
sxhkd -c ~/.config/custard/sxhkd &
(sleep 2; sh ~/.config/custard/rc.sh) &
custard 2>> /tmp/custard.log
EOF
    chmod +x ~/.config/custard/start.sh
}

inform(){
    cat <<EOF
custard has been installed on your system.
To use custard, comment out any WM or DE being started in
~/.xinitrc, and add the following line:
    \`~/.config/custard/start.sh\`

Ensure that there is no display manager running or enabled,
or if there is, you copy the .desktop file from the contrib
directory to the appropriate location for your display manager,
otherwise, it will take precedence over ~/.xinitrc.

When you login, invoke \`startx\`.
EOF
}

main(){
    checkdepends
    installandconfigure
    inform
}

main
