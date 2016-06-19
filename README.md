# SubmarinoColetorLixoModuloKernel

### Instrucoes

#### Instalar

- sudo mknod /dev/so c 50 0
- sudo chmod 666 /dev/so
- sudo insmod driver_so.ko

#### Remover

- sudo rmmod driver_so.ko
- sudo rm /dev/so

#### Testado com

* gcc 5.3.1
* fedora 23
* sfml 2.3.2
