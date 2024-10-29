import pyrogue as pr
import rogue

def parseEventFrame(frame):
    channel = frame.getChannel()
    fl = frame.getPayload()
    raw = bytearray(fl)
    frame.read(raw, 0)

    bunchCount = raw[0]
    pulseId = int.from_bytes(raw[1:9], 'little', signed=False)
    contributorId = raw[9]
    subsystemId = raw[10]

    print('Got frame')
        print(f'{subsystemId=}, {contributorId=}')
        print(f'{pulseId=}, {bunchCount=}')
        
    

class TsRawEventReceiver(rogue.interfaces.stream.Slave):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def _acceptFrame(self, frame):

        data = parseEventFrame(frame)

        print('Data')
        for ch in data:
            print(hex(ch))
