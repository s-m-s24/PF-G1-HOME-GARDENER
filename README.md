# BLE en ESP32

### For starters: BLE = Bluetooth Low Energy

## ¿Cuál es la diferencia entre el Bluetooth y el BLE?

Lo que nos sirve del BLE, es que no se mantiene constantemente prendido, sino que se mantiene en _**sleep mode**_, es decir, "dormido" constantemente hasta que se inicia la conexión. Esto hace que se consuma menos energía (consume _100 veces menos_ que el Bluetooth normal).

* En este caso, nos conviene más el BLE, ya que no vamos a intercambiar grandes cantidades de datos entre microcontroladores.


## BLE: Servidor y Cliente

En el BLE hay dos tipos de dispositivos: el **Servidor** y el **Cliente**. Un ESP32 puede actuar tanto como cliente como servidor.

* **Servidor**: El servidor _advierte de su existencia_, para que los otros dispositivos puedan verlos, y contiene datos que el **cliente** puede ver.

* **Cliente**: El cliente _escanea los dispositivos que encuentra cerca_ hasta encontrar a su servidor. Cuando esto sucede, establece la conexión y "_escucha_" los datos que envía el servidor.

Tenemos _**dos modos**_ de comunicación:

* **Broadcast mode**: El servidor transmite datos a _varios clientes que están conectados_.

* **Mesh network**: Todos los dispositivos están conectados, siendo una _many to many connection_.

## GATT

**GATT** significa _Generic Attributes_ y define cómo los dispositivos BLE envían y reciben mensajes.

(https://www.google.com/imgres?q=gatt%20ble&imgurl=https%3A%2F%2Fi0.wp.com%2Frandomnerdtutorials.com%2Fwp-content%2Fuploads%2F2018%2F06%2FGATT-BLE-ESP32.png%3Fresize%3D750%252C421%26quality%3D100%26strip%3Dall%26ssl%3D1&imgrefurl=https%3A%2F%2Frandomnerdtutorials.com%2Fesp32-ble-server-environmental-sensing-service%2F&docid=P01ZWubwoDMh1M&tbnid=zeD9tJ8E_LxS7M&vet=12ahUKEwiGt6vv7-KPAxUBq5UCHQJmCu4QM3oECCAQAA..i&w=750&h=421&hcb=2&ved=2ahUKEwiGt6vv7-KPAxUBq5UCHQJmCu4QM3oECCAQAA)