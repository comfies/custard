#!/bin/bash

doesntexist(){
    echo "$1 is not installed on the system. Please install the package that provides $1 and any needed dependencies."
    echo "It is recommended that you refer to your distro's repositories to install this before installing from any unknown or untrusted sources."
    exit 1
}

checkdepends(){
    command -v make > /dev/null || doesntexit "make"
    #just in case?
    command -v g++ > /dev/null || doesntexist "g++"
    command -v python3 > /dev/null || doesntexist "python3"
}

installandconfigure(){

    make
    sudo make install

    mkdir -p ~/.config/custard
    cp ./examples/sxhkdrc ~/.config/custard/sxhkdrc
    cp ./examples/Xresources ~/.config/custard/Xresources

    echo -e "#!/bin/bash\n" > ~/.config/custard/start.sh
    chmod +x ~/.config/custard/start.sh

    echo "echo '' > /tmp/custard.log" >> ~/.config/custard.start.sh
    echo "xrdb -merge ~/.config/custard/Xresources" >> ~/.config/custard/start.sh
    echo "sxhkd -c ~/.config/custard/sxhkdrc &" >> ~/.config/custard/start.sh
    echo "custard 2>> /tmp/custard.log" >> ~/.config/custard/start.sh

}

inform(){
    echo "custard has been installed on your system."
    echo "To use custard, comment out any WM or DE being started in"
    echo "~/.xinitrc, and add the following line:"
    echo "\t\`~/.config/custard/start.sh\`"
    echo -e "\nEnsure that there is no display manager running or enabled,"
    echo "otherwise, they will take precedence over ~/.xinitrc."
    echo "When you login, invoke \`startx\`."
}

main(){
    checkdepends
    installandconfigure
    inform
}

main
