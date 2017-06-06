# TinuOS
This is my road to osdev.


## Try it out
**Dependencies**
You will need a few dependencies: ['git', 'make', 'gcc', 'build-essential',...]

In debian based systems you can obtain the dependencies using:

    sudo apt-get install git make gcc build-essential

**Cloning the project source**
Get the source using git by issuing:

    git clone https://github.com/ashishhacker/TinuOS
    cd TinuOS

**Build the project**
Use the make command and to test out the kernel immediately using QEMU/KVM use the following commands:

    make
    cd iso/boot
    qemu-system-x86_64 --kernel kernel
