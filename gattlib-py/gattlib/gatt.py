from gattlib import *
from .uuid import gattlib_uuid_to_uuid, gattlib_uuid_to_int
from .exception import handle_return


class GattService():

    def __init__(self, device, gattlib_primary_service):
        self._device = device
        self._gattlib_primary_service = gattlib_primary_service

    @property
    def uuid(self):
        return gattlib_uuid_to_uuid(self._gattlib_primary_service.uuid)

    @property
    def short_uuid(self):
        return gattlib_uuid_to_int(self._gattlib_primary_service.uuid)


class GattCharacteristic():

    def __init__(self, device, gattlib_characteristic):
        self._device = device
        self._gattlib_characteristic = gattlib_characteristic

    @property
    def uuid(self):
        return gattlib_uuid_to_uuid(self._gattlib_characteristic.uuid)

    @property
    def short_uuid(self):
        return gattlib_uuid_to_int(self._gattlib_characteristic.uuid)

    @property
    def connection(self):
        return self._device.connection

    def read(self, callback=None):
        if callback:
            raise RuntimeError("Not supported yet")
        else:
            _buffer = c_void_p(None)
            _buffer_len = c_size_t(0)

            ret = gattlib_read_char_by_uuid(self.connection, self._gattlib_characteristic.uuid, byref(_buffer), byref(_buffer_len))

            pointer_type = POINTER(c_byte * _buffer_len.value)
            c_bytearray = cast(_buffer, pointer_type)

            value = bytearray(_buffer_len.value)
            for i in range(_buffer_len.value):
                value[i] = c_bytearray.contents[i]

            return value

    def write(self, data):
        buffer_type = c_char * len(data)
        buffer = data
        buffer_len = len(data)

        ret = gattlib_write_char_by_uuid(self.connection, self._gattlib_characteristic.uuid, buffer_type.from_buffer_copy(buffer), buffer_len)
        handle_return(ret)

    def register_notification(self, callback, user_data=None):
        self._device._notification_add_gatt_characteristic_callback(self, callback, user_data)

    def notification_start(self):
        ret = gattlib_notification_start(self.connection, self._gattlib_characteristic.uuid)
        handle_return(ret)

    def notification_stop(self):
        ret = gattlib_notification_stop(self.connection, self._gattlib_characteristic.uuid)
        handle_return(ret)

    def __str__(self):
        return str(self.uuid)