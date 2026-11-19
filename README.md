# pidp1-tools

This is a collection of tools for use with the pidp1-mods PDP-1 emulator in the pidp1-mods repository.

It can be checked out anywhere, but then copying the directory for the tool or tools you want to use to the
/opt/pidp1-mods/Tools directory in a pidp1-mods clone is the logical place to build and install from.

Any installs the tools here do expect the usual /opt/pidp1-mods to exist, and they require a full pidp1-mods
installation since they use am1 and other pidp1-mods tools.

The suggested install process is to install into /opt:

```
cd /opt
sudo mkdir pidp1-tools
sudo chown youruser pidp1-tools
git clone --depth 1 https://github.com/wjenh/pidp1-tools.git
```
This avoids checking out all the history that will just use up space
and sets ownership to you.
