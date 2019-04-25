Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/xenial64"

  config.vm.provider "virtualbox" do |vb|
    # USB 1.0 ("Low"|"Full")
    #vb.customize ["modifyvm", :id, "--usb", "on"]

    # USB 2.0 ("High")
    #vb.customize ["modifyvm", :id, "--usbehci", "on"]

    # USB 3.0 ("Super")
    vb.customize ["modifyvm", :id, "--usbxhci", "on"]

    vb.customize ["usbfilter", "add", "0",
      "--target", :id,
      "--name", "RSA",
      "--vendorid", "0x0699",
      "--productid", "0xa306"]
  end

  config.vm.provision "shell", :inline => <<-SHELL
    apt-get update && \
    apt-get install -y \
    ubuntu-desktop

  SHELL

  config.vm.provision "shell", :inline => <<-SHELL
    /vagrant/setup_build_env.sh

  SHELL
end

