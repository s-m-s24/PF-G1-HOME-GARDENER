### BLE en ESP32

#For starters: BLE = Bluetooth Low Energy

## ¿Cuál es la diferencia entre el Bluetooth y el BLE?

*Lo que nos sirve del BLE, es que no se mantiene constantemente prendido, sino que se mantiene en **_sleep mode_**, es decir, "dormido" constantemente hasta que se inicia la conexión. Esto hace que se consuma menos energía (consume _100 veces menos_ que el Bluetooth normal).

*En este caso, nos conviene más el BLE, ya que no vamos a intercambiar grandes cantidades de datos entre microcontroladores.


## BLE: Servidor y Cliente

En el BLE hay dos tipos de dispositivos: el **Servidor** y el **Cliente**. Un ESP32 puede actuar tanto como cliente como servidor.

* **Servidor**:
