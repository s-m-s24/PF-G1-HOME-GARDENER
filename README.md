# BLE en ESP32

* [**Mesh Connection ESP32 BLE**](/.BLE_ESP.md)

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

*imagen gatt *

## Characteristic en el BLE

La _characteristic_ va a pertenecer siempre al servidor

## UUID

Cada dispositivo o characteristic va a tener un **UUID** (_Universally Unique Identifier_). Un UUID es una secuencia única de _128 bits_ (16 bytes). Por ejemplo:

`55072829-bc9e-4c53-938a-74a6d4c78776`

Esto nos va a permitir que los dispositivos se identifiquen entre sí y puedan intercambiar los datos.

`#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"` 
`#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" `

Para crear tus propios UUIDs, visita https://www.uuidgenerator.net/ 

