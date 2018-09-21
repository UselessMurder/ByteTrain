import struct

class BTTypeCode:
    BT_CODE_BYTE = 1
    BT_CODE_WORD = 2
    BT_CODE_DWORD = 3
    BT_CODE_QWORD = 4
    BT_CODE_BYTE_BUFFER = 5
    BT_CODE_WORD_BUFFER = 6
    BT_CODE_DWORD_BUFFER = 7
    BT_CODE_QWORD_BUFFER = 8
    BT_CODE_HANDLER_BEGIN = 9
    BT_CODE_HANDLER_END = 10
    BT_CODE_BREAK = 11

class BTUserType:
    BT_U_CODE_BYTE = 1
    BT_U_CODE_WORD = 2
    BT_U_CODE_DWORD = 3
    BT_U_CODE_QWORD = 4
    BT_U_CODE_BUFFER = 5
    BT_U_CODE_ARRAY = 6
    BT_U_CODE_ERROR = 7

class BTMagicCode:
    BT_CHAIN_MAGIC = 1
    BT_ARRAY_MAGIC = 2

class BTErrorCode:
    BT_ERROR_SUCCESS = 0
    BT_ERROR_TIMEOUT = 1

class WaitOver(Exception):
    def __init__(self, message, errors):
        super(WaitOver, self).__init__(message)
        self.errors = errors

class BTContext(object):
    def __init__(self, in_break):
        self.break_level = 0
        self.count = 0
        self.errors = None
        self.in_break = in_break

    def add_error(self, error):
        if self.errors is None:
            self.errors = dict()
        self.errors[self.count] = error
        self.break_level -= 1

    def check_break(self):
        if self.break_level != 0:
            self.break_level -= 1
            return True
        return False

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
                if not self.wait_object.Wait():
                    raise WaitOver(self.wait_object.get_reason(), self.wait_object.get_code())
                continue
            return content

    def __read_type(self):
        content = self.__read_bytes(1)
        code = struct.unpack('<B', content)
        return code

    def __run_with_increment(code, ctx):
        ctx.count += 1
        return self.default_handlers[code](ctx)


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
                (BTMagicCode.BT_ARRAY_MAGIC, self.__read_array)
                ]}
        return handlers

    def read_message(self):
        ctx = BTContext(False)
        (val, code) = self.__run_with_increment(self.__read_type(), ctx)
        if ctx.break_level != 0 or ctx.in_break or ctx.depth != 0:
            raise RuntimeError('Broken ctx %s' % ctx)
        return (val, code, ctx.errors)
            
    def __read_byte(self, ctx):
        return (struct.unpack('<B', self.__read_bytes(1)), BTUserType.BT_U_CODE_BYTE)

    def __read_word(self, ctx):
        return (struct.unpack('<H', self.__read_bytes(2)), BTUserType.BT_U_CODE_WORD)

    def __read_dword(self, ctx): 
        return (struct.unpack('<L', self.__read_bytes(4)), BTUserType.BT_U_CODE_DWORD)
    
    def __read_qword(self, ctx): 
        return (struct.unpack('<Q', self.__read_bytes(8)), BTUserType.BT_U_CODE_QWORD)

    def __read_byte_buffer(self, ctx):
        size = struct.unpack('<B', self.__read_bytes(1))
        return (self.__read_bytes(size), BTUserType.BT_U_CODE_BUFFER)

    def __read_word_buffer(self, ctx):
        size = struct.unpack('<H', self.__read_bytes(2))
        return (self.__read_bytes(size), BTUserType.BT_U_CODE_BUFFER)
    
    def __read_dword_buffer(self, ctx):
        size = struct.unpack('<L', self.__read_bytes(4))
        return (self.__read_bytes(size), BTUserType.BT_U_CODE_BUFFER)

    def __read_qword_buffer(self, ctx):
        size = struct.unpack('<Q', self.__read_bytes(8))
        return (self.__read_bytes(size), BTUserType.BT_U_CODE_BUFFER)

    def __read_embedded_handler_begin(self, ctx):
        magic = struct.unpack('<H', self.__read_bytes(2))
        return self.embedded_handlers[magic](ctx)

    def __read_embedded_handler_end(self, ctx):
        pass 

    def __read_break(self, ctx):
        if ctx.in_break:
            raise RuntimeError('Nested break')
        content = self.read_bytes(2)
        level = struct.unpack('<B', content[0:1])
        count = struct.unpack('<B', content[1:2])
        error_content = []
        ctx.break_level = level
        break_ctx = BTContext(True)
        for i in range(count):
            error_content.append(self.__run_with_increment(self.__read_type(), break_ctx))
        return (error_content, BTUserType.BT_U_CODE_ERROR)

    def __read_chain(self, ctx):
        chain = ''
        while True:
            code = self.__read_type()
            if code >= BTTypeCode.BT_CODE_BYTE_BUFFER and code <= BTTypeCode.BT_CODE_QWORD_BUFFER:
                chain += self.default_handlers[code](ctx)[0]
            elif code == BTTypeCode.BT_CODE_BREAK:
                ctx.add_error(self.default_handlers[code](ctx))
                break
            elif code == BTTypeCode.BT_CODE_HANDLER_END:
                self.default_handlers[code](ctx)
                break
            else:
                raise RuntimeError('Unpredictable chain')
        return (chain, BTUserType.BT_U_CODE_BUFFER)

    def __read_array(self, ctx):
        array = []
        while True:
            code = self.__read_type() 
            if code == BTTypeCode.BT_CODE_HANDLER_END:
                self.default_handlers[code](ctx)
                break  
            elif code == BTTypeCode.BT_CODE_BREAK:
                ctx.add_error(self.default_handlers[code](ctx))
                break
            else:
                array.append(self.__run_with_increment(code, ctx))
                if ctx.check_break():
                    break
        return (array, BTUserType.BT_U_CODE_ARRAY)
