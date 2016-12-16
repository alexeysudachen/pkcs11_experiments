
import sys, os, os.path, re, StringIO

TRANSMITTED = '>>'
RECEIVED = '<<'

def textprint(state,fot,at,end):
    fot.write('|')
    for j in range(at,end):
        fot.write(' ')
        if state[j] >= 0x30 and state[j] <= 0x7f:
            fot.write(chr(state[j]))
        else:
            fot.write('.')
        
def hexprint(pfx,state,fot):
    WIDTH = 16
    fot.write(pfx + '00 |')
    for i, num in enumerate(state):
        if ( i % WIDTH == 0 and i != 0 ):
            textprint(state,fot,i-WIDTH,i)
            fot.write('\n'+pfx+'%02x |'%i)
        elif i != 0:
            fot.write(' ')
        fot.write("%02x"%num)
    foo = len(state) % WIDTH
    if foo == 0:
        foo = WIDTH
    else:
        fot.write("   "*(WIDTH-foo))
    textprint(state,fot,len(state) - foo,len(state))
    fot.write('\n')
    
def flush(direction,state,fot):
    if direction and state:     
        if direction == RECEIVED:
            SW2 = state.pop()
            SW1 = state.pop()
            fot.write('{SW1: %02x, SW2: %02x}\n-\n'%(SW1,SW2))
        elif direction == TRANSMITTED:
            fot.write('-------\n')
            CLA = state[0]
            CMD = state[1]
            P1  = state[2]
            P2  = state[3]
            LC  = state[4]
            if len(state) == 5:
                LE = LC
                LC = 0
            else:
                if len(state) > 5+LC:
                    LE = state[5+LC]
                else:
                    LE = 0
            state = state[5:5+LC]
            fot.write("{CLA: %02x, CMD: %02x, P1: %02x, P2: %02x, LC: %02x, LE %02x}\n"%(CLA,CMD,P1,P2,LC,LE))
            if LC != 0: fot.write('-\n')
        if len(state) != 0:
            hexprint('+',state,fot)
        if direction == RECEIVED:
            fot.write('\n')

def convert(fin,fot):
    state = []
    direction = None
    for l in fin:
        if re.search("transmitted",l):
            flush(direction,state,fot)
            state = []
            direction = TRANSMITTED
        elif re.search("received",l):
            flush(direction,state,fot)
            state = []
            direction = RECEIVED
        else:
            l = l.strip()
            if len(l) and l[0] in "0123456789abcdefABCDEF" :
                state.extend(map(lambda x: int(x,16), l.split()))
    flush(direction,state,fot)

convert(sys.stdin,sys.stdout)            

