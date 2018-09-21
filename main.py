import bt
import sys



class reader(object):
    def __init__(self, path):
        self.path = path
        self.f = None

    def open(self):
        self.f = open(self.path, "rb")

    def close(self):
        self.f.close()

    def read(self, count):
       content = self.f.read(count)
       if len(content) != count:
           return None, bt.BTErrorCode.BT_ERROR_TIMEOUT
       return content, bt.BTErrorCode.BT_ERROR_SUCCESS
    
    def __enter__(self):
        self.open()
        return self

    def __exit__(self, val, type, tb):
        self.close()

class NoWait(object):
    def __init__(self):
        pass
    def wait(self):
        return False

    def get_reason(self):
        return "No wait"

    def get_code(self):
        return 0

if __name__ == "__main__":
    with reader("hello") as r:
        b_t = bt.ByteTrain(r, NoWait())
        while True:
            try:
                print(b_t.read_message())
                sys.stdin.read(1)
            except bt.WaitOver as w:
                print(w)
                break


