import struct

class BTTypeCode:
    BT_CODE_BYTE          = 1
    BT_CODE_WORD          = 2
    BT_CODE_DWORD         = 3
    BT_CODE_QWORD         = 4
    BT_CODE_BYTE_BUFFER   = 5
    BT_CODE_WORD_BUFFER   = 6
    BT_CODE_DWORD_BUFFER  = 7
    BT_CODE_QWORD_BUFFER  = 8
    BT_CODE_HANDLER_BEGIN = 11
    BT_CODE_HANDLER_END   = 12
    BT_CODE_BREAK         = 13

class BTMagicCode:
    BT_CHAIN_MAGIC  = 1
    BT_STRUCT_MAGIC = 2

class BTErrorCode:
    BT_ERROR_SUCCESS      = 0
    BT_ERROR_TIMEOUT      = 1
    BT_ERROR_NO_MORE_WAIT = 3

class WaitOver(Exception):
    def __init__(self, message, errors):
        super(WaitOver, self).__init__(message)
        self.errors = errors

class ByteTrain(object):
    def __init__(self, reader, wait_object):
        self.reader = reader
        self.wait_object = wait_object
        self.default_handlers = self.__init_default_handlers()
        self.embedded_handlers = self.__init_embedded_handlers()

    def __read_bytes(self, count):
        while True:
            content, code = self.reader(count)
            if code == BTErrorCode.BT_ERROR_TIMEOUT:
                if !self.wait_object.Wait():
                    raise WaitOver(self.wait_object.get_reason(), self.wait_object.get_code())
                continue
            return content

    def __read_type(self):
        content = self.__read_bytes(1)
        code = struct.unpack("<B", content)
        return code

    def __init_default_handlers(self):
        handlers = {[
                (BTTypeCode.BT_CODE_BYTE, self.__read_byte),
                (BTTypeCode.BT_CODE_WORD, self.__read_word),
                (BTTypeCode.BT_CODE_DWORD, self.__read_dword),
                (BTTypeCode.BT_CODE_QWORD, self.__read_qword),
                (BTTypeCode.BT_CODE_BYTE_BUFFER, self.__read_byte_buffer),
                (BTTypeCode.BT_CODE_WORD_BUFFER, self.__read_word_buffer),
                (BTTypeCode.BT_CODE_DWORD_BUFFER, self.__read_dword_buffer),
                (BTTypeCode.BT_CODE_QWORD_BUFFER, self.__read_qword_buffer),
                (BTTypeCode.BT_CODE_HANDLER_BEGIN, self.__read_embedded_handler_begin),
                (BTTypeCode.BT_CODE_HANDLER_END, self.__read_embedded_handler_end),
                (BTTypeCode.BT_CODE_BREAK, self.__read_break)
        ]}
        return handlers

    def __init_embedded_handlers(self):
        handlers = {[
                (BTMagicCode.BT_CHAIN_MAGIC, self.__read_chain),
                (BTMagicCode.BT_STRUCT_MAGIC, self.__read_struct)
                ]}
        return handlers

    def read_message(self):
        handler_code = self.__read_type()
        return self.default_handlers[handler_code]()

    def __read_byte(self):
        content = self.__read_bytes(1)
        return (struct.unpack("<B", content), BTTypeCode.BT_CODE_BYTE, None)

    def __read_word(self):
        content = self.__read_bytes(2)
        return (struct.unpack("<H", content), BTTypeCode.BT_CODE_WORD, None)

    def __read_dword(self):
        content = self.__read_bytes(4)
        return (struct.unpack("<L", content), BTTypeCode.BT_CODE_DWORD, None)
    
    def __read_qword(self):
        content = self.__read_bytes(8)
        return (struct.unpack("<Q", content), BTTypeCode.BT_CODE_QWORD, None)

    def __read_byte_buffer(self):
        size = self.__byte_handler()
        content = self.__read_bytes(size)
        return (content, BTTypeCode.BT_CODE_BYTE_BUFFER, None)

    def __read_word_buffer(self):
        size = self.__word_handler()
        content = self.__read_bytes(size)
        return (content, BTTypeCode.BT_CODE_WORD_BUFFER, None)
    
    def __read_dword_buffer(self):
        size = self.__dword_handler()
        content = self.__read_bytes(size)
        return (content, BTTypeCode.BT_CODE_DWORD_BUFFER, None)

    def __read_qword_buffer(self):
        size = self.__qword_handler()
        content = self.__read_bytes(size)
        return (content, BTTypeCode.BT_CODE_QWORD_BUFFER, None)

    def __read_embedded_handler_begin(self):
        magic = self.__word_handler()
        return self.embedded_handlers[magic]

    def __read_embedded_handler_end(self):
        pass 

    def __read_break(self):
       content = self.read_bytes(2)
       level = struct.unpack("<B", content[0:1])
       count = struct.unpack("<B", content[1:2])
       break_message = []
       break_message.append((level, BTTypeCode.BT_CODE_BYTE, None))
       for i in range(count):
           break_message.append(self.read_message())
       return (None, BTTypeCode.BT_CODE_BREAK, break_message) 

   def __read_chain(self):
       pass

   def __read_struct(self):
       pass

