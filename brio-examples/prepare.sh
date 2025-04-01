# This scriot provides some quick configure and install commands useful
# for development.
alias brio-config='./configure.sh -I -CP 192.168.129.0 --no-interactive Y'
alias brio-unconfig='./configure.sh -R --no-interactive Y'
alias brio-install='./install.sh -I'
alias brio-unintall='./uninstall.sh install.sh.log --no-interactive Y'