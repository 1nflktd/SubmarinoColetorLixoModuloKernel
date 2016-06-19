# SubmarinoColetorLixoModuloKernel

### Instrucoes

#### Instalar

- sudo mknod /dev/so c 50 0
- sudo chmod 666 /dev/so
- sudo insmod driver_so.ko

#### Remover

- sudo rmmod driver_so.ko
- sudo rm /dev/so
