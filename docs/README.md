[![TekMonogram](https://tektronix.github.io/media/tekmonogram.png)](https://github.com/tektronix)

# gr-tekrsa
[![Tektronix](https://tektronix.github.io/media/TEK-opensource_badge.svg)](https://github.com/tektronix) [![CodeFactor](https://www.codefactor.io/repository/github/tektronix/usb-rsa-gnuradio-module-wip/badge)](https://www.codefactor.io/repository/github/tektronix/usb-rsa-gnuradio-module-wip) [![Total alerts](https://img.shields.io/lgtm/alerts/g/tektronix/USB-RSA-gnuradio-module-WIP.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/tektronix/USB-RSA-gnuradio-module-WIP/alerts/) [![Build Status](https://travis-ci.com/tektronix/USB-RSA-gnuradio-module-WIP.svg?branch=master)](https://travis-ci.com/tektronix/USB-RSA-gnuradio-module-WIP)

## Disclaimer
This project is a work in progress and has not reached a stable v1.0 release yet.

## Description
A GNU Radio Module for use with the Tektronix USB RSA products. What is a GNU Radio Module? Check out the community [gnuradio.org](https://www.gnuradio.org/)

For more information on the directory structure, check out the wiki: [Structure of a module](https://wiki.gnuradio.org/index.php/OutOfTreeModules#Structure_of_a_module)

## Maintainers
- Kyle Bernard: [kyleb3522](https://github.com/kyleb3522)
- David Huddleson [huddlesd](https://github.com/huddlesd)

## Community Forum
Come chat with us on slack! [Slack gr-tekrsa Community Sign Up](https://join.slack.com/t/gr-tekrsa/shared_invite/enQtNzIyOTg1MDg3MDU5LTkzNjg3ZWIxOWQwMmQ2YjU2OTUxNmNjZmVkZmM2ZTRjYzc5MGUyMjhmMjczYzJkZDVjZDA1YWUzOWQ4MWJlNTE)

## Production Environment
This project is meant to be eveloped and install on Linux. If you are not running Linux on your computer, you can run a Vagrant instance (A container system) to run linux. A Vagrantfile is provided in this repo. 
1. Install Vagrant on your Mac/Windows/Linux system [Vagrant Install Instructions](https://www.vagrantup.com/intro/getting-started/install.html)
2. Run the Vagrantfile in this repo [Vagrant Run Instructions](https://www.vagrantup.com/intro/getting-started/up.html)
3. Now you have a linux system that has run a script to install the latest version of this code.

## Build and Install
How to get the GNURadio Companion Tek RSA demo running:

0. Download the RSA_API.tar.gz file here: [RSA_API.tar.gz](https://github.com/tektronix/USB-RSA-gnuradio-module-WIP/releases/download/v0.1.2-alpha/RSA_API-0.13.211.tar.gz)
1. Run the command `sudo ./setup_build_env.sh` in the terminal. *Note: Previous step is required for this script to succeed.*
	- This script installs many dependencies and is expected to take
	several minutes. You will know the script finished successfully if
	the last line it prints says "Success: setup complete".

2. When that finishes, close the terminal that ran step 1 and open a new terminal.

3. In the new terminal, return to the project root directory
	and enter the command "gnuradio-companion" to verify
	the base program installed correctly. Then exit GNURadio Companion.

4. Enter the command `gedit ~/.gnuradio/grc.conf`.
	This will open the file "grc.conf" which will be edited in step 5.

5. Add the following lines to the bottom of the grc.conf file:
```
[grc]
local_blocks_path = /usr/local/share/gnuradio/grc/blocks/
```
6. Save the file changes and close the file.

7. Run the demo script `grc_RSA_demo.sh` in the "examples" directory.
	- Run it with super-user permission. 
	- This should open GNURadio Companion showing a simple setup
	- using the Tek RSA blocks.

8. Plug the RSA300 into the computer.

9. In GNURadio Companion, run the setup by clicking Run -> Execute in the toolbar,
	or by clicking the green triangle. A warning message may pop up, but this
	can be dismissed. If everything was set up correctly, you should see
	a Frequency vs. Power graph that is updating using output from the RSA.

## Use
This is the TekRSA-write-a-block package meant as a guide to building
out-of-tree packages. To use the TekRSA blocks, the Python namespaces
is in 'TekRSA', which is imported as:

    `import TekRSA`

See the Doxygen documentation for details about the blocks available
in this package. A quick listing of the details can be found in Python
after importing by using:

    `help(TekRSA)`

## Contributing
First, please consult the Tektronix [Code of Conduct](https://tektronix.github.io/Code-Of-Conduct/). Contributions in the form of new examples or bug fixes are welcome! Please contribute using [Github Flow](https://guides.github.com/introduction/flow/) and make sure you create your own branch from the `master` branch, and when you are ready to push your changes, just submit a pull request with your changes against the `master` branch. If you see something you're not sure is a bug, or if you'd like to request an example, please submit an Issue via GitHub [New Issue](https://github.com/tektronix/USB-RSA-gnuradio-module-WIP/issues/new).

### Contributor License Agreement
Contributions to this project must be accompanied by a Contributor License Agreement. You (or your employer) retain the copyright to your contribution; this simply gives us permission to use and redistribute your contributions as part of the project.

You generally only need to submit a CLA once, so if you've already submitted one (even if it was for a different [Tektronix](https://github.com/tektronix/) project), you probably don't need to do it again.
