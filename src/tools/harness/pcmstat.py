import struct, sys
raw = open(sys.argv[1], 'rb').read()
n = len(raw) // 2
s = struct.unpack('<%dh' % n, raw[:n*2])
# stereo interleaved: take left channel
L = s[0::2]
nz = [i for i, v in enumerate(L) if v != 0]
if not nz: print("silent"); sys.exit()
a, b = nz[0], nz[-1]
seg = L[a:b+1]
jumps = [abs(seg[i+1]-seg[i]) for i in range(len(seg)-1)]
big = sum(1 for j in jumps if j > 20000)
print(f"samples={len(L)} nonzero_span={len(seg)} ({len(seg)/11025:.2f}s) min={min(seg)} max={max(seg)} maxjump={max(jumps)} jumps>20000={big}")
