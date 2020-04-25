import numpy as np
import matplotlib.pyplot as plt
import time

qam64_syms = [3/np.sqrt(42) + 1j*3/np.sqrt(42),
              3/np.sqrt(42) + 1j*1/np.sqrt(42),
              1/np.sqrt(42) + 1j*3/np.sqrt(42),
              1/np.sqrt(42) + 1j*1/np.sqrt(42),
              3/np.sqrt(42) + 1j*5/np.sqrt(42),
              3/np.sqrt(42) + 1j*7/np.sqrt(42),
              1/np.sqrt(42) + 1j*5/np.sqrt(42),
              1/np.sqrt(42) + 1j*7/np.sqrt(42),
              5/np.sqrt(42) + 1j*3/np.sqrt(42),
              5/np.sqrt(42) + 1j*1/np.sqrt(42),
              7/np.sqrt(42) + 1j*3/np.sqrt(42),
              7/np.sqrt(42) + 1j*1/np.sqrt(42),
              5/np.sqrt(42) + 1j*5/np.sqrt(42),
              5/np.sqrt(42) + 1j*7/np.sqrt(42),
              7/np.sqrt(42) + 1j*5/np.sqrt(42),
              7/np.sqrt(42) + 1j*7/np.sqrt(42),
              3/np.sqrt(42) - 1j*3/np.sqrt(42),
              3/np.sqrt(42) - 1j*1/np.sqrt(42),
              1/np.sqrt(42) - 1j*3/np.sqrt(42),
              1/np.sqrt(42) - 1j*1/np.sqrt(42),
              3/np.sqrt(42) - 1j*5/np.sqrt(42),
              3/np.sqrt(42) - 1j*7/np.sqrt(42),
              1/np.sqrt(42) - 1j*5/np.sqrt(42),
              1/np.sqrt(42) - 1j*7/np.sqrt(42),
              5/np.sqrt(42) - 1j*3/np.sqrt(42),
              5/np.sqrt(42) - 1j*1/np.sqrt(42),
              7/np.sqrt(42) - 1j*3/np.sqrt(42),
              7/np.sqrt(42) - 1j*1/np.sqrt(42),
              5/np.sqrt(42) - 1j*5/np.sqrt(42),
              5/np.sqrt(42) - 1j*7/np.sqrt(42),
              7/np.sqrt(42) - 1j*5/np.sqrt(42),
              7/np.sqrt(42) - 1j*7/np.sqrt(42),
              -3/np.sqrt(42) + 1j*3/np.sqrt(42),
              -3/np.sqrt(42) + 1j*1/np.sqrt(42),
              -1/np.sqrt(42) + 1j*3/np.sqrt(42),
              -1/np.sqrt(42) + 1j*1/np.sqrt(42),
              -3/np.sqrt(42) + 1j*5/np.sqrt(42),
              -3/np.sqrt(42) + 1j*7/np.sqrt(42),
              -1/np.sqrt(42) + 1j*5/np.sqrt(42),
              -1/np.sqrt(42) + 1j*7/np.sqrt(42),
              -5/np.sqrt(42) + 1j*3/np.sqrt(42),
              -5/np.sqrt(42) + 1j*1/np.sqrt(42),
              -7/np.sqrt(42) + 1j*3/np.sqrt(42),
              -7/np.sqrt(42) + 1j*1/np.sqrt(42),
              -5/np.sqrt(42) + 1j*5/np.sqrt(42),
              -5/np.sqrt(42) + 1j*7/np.sqrt(42),
              -7/np.sqrt(42) + 1j*5/np.sqrt(42),
              -7/np.sqrt(42) + 1j*7/np.sqrt(42),
              -3/np.sqrt(42) - 1j*3/np.sqrt(42),
              -3/np.sqrt(42) - 1j*1/np.sqrt(42),
              -1/np.sqrt(42) - 1j*3/np.sqrt(42),
              -1/np.sqrt(42) - 1j*1/np.sqrt(42),
              -3/np.sqrt(42) - 1j*5/np.sqrt(42),
              -3/np.sqrt(42) - 1j*7/np.sqrt(42),
              -1/np.sqrt(42) - 1j*5/np.sqrt(42),
              -1/np.sqrt(42) - 1j*7/np.sqrt(42),
              -5/np.sqrt(42) - 1j*3/np.sqrt(42),
              -5/np.sqrt(42) - 1j*1/np.sqrt(42),
              -7/np.sqrt(42) - 1j*3/np.sqrt(42),
              -7/np.sqrt(42) - 1j*1/np.sqrt(42),
              -5/np.sqrt(42) - 1j*5/np.sqrt(42),
              -5/np.sqrt(42) - 1j*7/np.sqrt(42),
              -7/np.sqrt(42) - 1j*5/np.sqrt(42),
              -7/np.sqrt(42) - 1j*7/np.sqrt(42)]

qam16_syms = [1/np.sqrt(10) + 1j/np.sqrt(10), 1/np.sqrt(10) + 3j/np.sqrt(10),
            3/np.sqrt(10) + 1j/np.sqrt(10), 3/np.sqrt(10) + 3j/np.sqrt(10),
            1/np.sqrt(10) - 1j/np.sqrt(10), 1/np.sqrt(10) - 3j/np.sqrt(10),
            3/np.sqrt(10) - 1j/np.sqrt(10), 3/np.sqrt(10) - 3j/np.sqrt(10),
            -1/np.sqrt(10) + 1j/np.sqrt(10), -1/np.sqrt(10) + 3j/np.sqrt(10),
            -3/np.sqrt(10) + 1j/np.sqrt(10), -3/np.sqrt(10) + 3j/np.sqrt(10),
            -1/np.sqrt(10) - 1j/np.sqrt(10), -1/np.sqrt(10) - 3j/np.sqrt(10),
            -3/np.sqrt(10) - 1j/np.sqrt(10), -3/np.sqrt(10) - 3j/np.sqrt(10)]

qam4_syms = [np.exp(np.pi*5/4*1j), np.exp(np.pi*3/4*1j), 
            np.exp(np.pi*7/4*1j), np.exp(np.pi*1/4*1j)]

qam2_syms = [np.exp(np.pi*5/4*1j), np.exp(np.pi*1/4*1j)]

fft_size = 32
bandwidth = 4e3
bits_per_sym = 2 #QPSK
subcarrier_width = bandwidth / fft_size
symbol_rate = subcarrier_width
sampling_rate = bandwidth
cp_samples = fft_size / 8

num_ref_sym = 8
ref_sym_spacing = fft_size / num_ref_sym
ref_sym_insertion = [i * ref_sym_spacing - i for i in range(num_ref_sym)]
ref_sym_indexes = [i * ref_sym_spacing for i in range(num_ref_sym)]
ref_sym = 1 + 0j
data_sym_per_slice = fft_size - num_ref_sym

def generateZCSequence(l):
    pn = list()
    nzc = l
    u = 1
    for n in range(nzc):
        pn.append(np.exp(-1j*np.pi*u*n*(n+np.mod(nzc, 2))/nzc))
    return pn

shortZC = generateZCSequence(fft_size / 4)
longZC = generateZCSequence(fft_size)
preamble_samps = longZC + 4 * shortZC

def symbolsToOFDM(syms):
    symbol_sets = []
    if len(syms) % data_sym_per_slice != 0:
        assert False

    for i in range(int(len(syms)/data_sym_per_slice)):
        ofdm_set = syms[i*data_sym_per_slice:(i+1)*data_sym_per_slice]
        
        # Insert Reference Symbols
        ofdm_set = np.insert(ofdm_set, ref_sym_insertion, [ref_sym]*num_ref_sym)


        ofdm_samps = np.fft.ifft(ofdm_set)
        cp_pre = ofdm_samps[-cp_samples:]
        ofdm_samps = np.concatenate((cp_pre,ofdm_samps))
        symbol_sets.append(ofdm_samps)

    all_samps = []
    for x in symbol_sets:
        all_samps.extend(x)

    return all_samps

def bitsToSymbols(b):
    syms = []

    m = dict()
    if bits_per_sym == 6:
        #64QAM
        m = qam64_syms
    if bits_per_sym == 4:
        #16QAM
        m = qam16_syms
    if bits_per_sym == 2:
        #QPSK
        m = qam4_syms
    if bits_per_sym == 1:
        #BPSK
        m = qam2_syms

    for i in range(int(np.ceil(len(b)/float(bits_per_sym)))):
        val = 0
        place = np.power(2,bits_per_sym-1)
        for bit in range(bits_per_sym):
            bit_idx = i*bits_per_sym+bit

            # Abort if run out of bits. Extra bits are the high bits.
            if bit_idx >= len(b):
                break

            val += b[bit_idx] * place
            place /= 2
        sym = m[val]
        syms.append(sym)

    # Pad syms out so that we have a integer number of slices
    still_needed = data_sym_per_slice - (len(syms) % data_sym_per_slice)
    if still_needed < data_sym_per_slice:
        syms.extend([m[0] * still_needed])
    
    return syms

def stringToBits(s):
    bits = []
    for c in s:
        num = ord(c)
        lsb = []
        for i in range(8):
            lsb.append(num % 2)
            num = int(np.floor(num / 2))
        msb = lsb[::-1]
        bits.extend(msb)
    return bits

def encodeOFDMFrame(d):
    bits = stringToBits(d)
    syms = bitsToSymbols(bits)
    samps = symbolsToOFDM(syms)
    return samps

def OFDMtoSymbols(time_syms):
    symbols = np.fft.fft(time_syms)
    return symbols

def fixWithReferenceSymbols(syms):
    refs = np.take(syms, ref_sym_indexes)
    print(np.angle(refs))
    rad_per_sym = np.average(np.diff(np.unwrap(np.angle(refs)))) / ref_sym_spacing
    print(rad_per_sym)
    unshift = np.multiply(syms, np.exp(np.linspace(0,fft_size-1,fft_size)*1j*-rad_per_sym))

    desired_ang = np.angle(ref_sym)
    cur_ang = np.angle(np.average(np.take(unshift, ref_sym_indexes)))
    return np.multiply(unshift, np.exp(1j*(desired_ang-cur_ang)))

def shiftOFDMFreq(syms, samp_shift):
    return np.multiply(syms, np.exp(np.linspace(0,fft_size-1,fft_size)*2*np.pi*1j*(float(samp_shift)/fft_size)))

def longZCSync(samps):
    peaks = []
    for i in range(len(samps) - len(longZC)):
        buf = samps[i:i+len(longZC)]
        buf_new = np.exp(np.multiply(np.angle(buf), 1j))
        peaks.append(np.sum(np.multiply(buf_new, np.conjugate(longZC))))
    return peaks

def findStart(peaks):
    for i in range(len(peaks)):
        if np.abs(peaks[i]) > fft_size*0.6:
            return i

def extractShortRefs(samples, start_id):
    sl = len(shortZC)
    ref_idxs = [start_id + len(longZC), 
        start_id + 1 * sl + len(longZC),
        start_id + 2 * sl + len(longZC),
        start_id + 3 * sl + len(longZC)]

    refs = []
    for i in ref_idxs:
        zc_buf = np.exp(np.multiply(np.angle(samples[i: i + sl]), 1j))
        peak = np.sum(np.multiply(zc_buf, np.conjugate(shortZC)))
        refs.append(peak)

    print(refs)
    return np.unwrap(np.angle(refs))

data = "hello world its me"*32
tx_iq = np.multiply(encodeOFDMFrame(data), 3)
#decode = shiftOFDMFreq(decode, 0)
#print(decode)
x = np.array(preamble_samps)
noisepower = .001
x = np.add(x, np.random.normal(size=len(x), scale=noisepower))
windup = np.exp(np.linspace(0,50-1,50)*2*np.pi*(200/bandwidth)*1j)
x = np.concatenate((np.random.normal(size=len(x), scale=noisepower), np.random.normal(size=len(x), scale=noisepower), windup,
    x, tx_iq, np.random.normal(size=len(x), scale=noisepower), np.random.normal(size=len(x), scale=noisepower)))


np.save("tx.npy", x)
phase_offset = 2
freq_offset = 3
x = np.multiply(x, np.exp(np.add(np.linspace(0,len(x)-1,len(x))*2*np.pi*(freq_offset/bandwidth), phase_offset) * 1j))

x = np.divide(x, 1)

x = np.fromfile("samps.bin", dtype=np.complex64)
x = x[len(x)*1/3:]
plt.plot(np.real(x))
plt.plot(np.imag(x))
plt.show()

peaks = longZCSync(x)
plt.plot(np.abs(peaks))
plt.show()
start_idx = findStart(peaks)
peak_val = peaks[start_idx]
# Phase indicates phase inbetween two middle samples
print("Found peak at index %d (%r, %.5f)" % (start_idx, peak_val, np.angle(peak_val)))

r = extractShortRefs(x, start_idx)
print(r)
freq_offset = np.average(np.diff(r)) / len(shortZC) / 2 / np.pi * bandwidth
print("Frequency offset of %.2f Hz determined" % freq_offset)

freq_corr = np.multiply(x, np.exp(np.linspace(0,len(x)-1,len(x))*2*np.pi*(-freq_offset/bandwidth * 1j)))
#freq_corr = x

buf = np.exp(np.multiply(np.angle(freq_corr[start_idx:start_idx+len(longZC)]), 1j))
fixed_peak = np.sum(np.multiply(buf, np.conjugate(longZC)))

#phase_offset = ((27 + 32) * (freq_offset/bandwidth) * 2 * np.pi) + np.angle(fixed_peak)
#phase_offset = ((phase_offset + np.pi) % (2 * np.pi)) - np.pi
#phase_offset = np.average(np.concatenate((extractShortRefs(freq_corr, start_idx), [np.angle(fixed_peak)])))
phase_offset = extractShortRefs(freq_corr, start_idx)[3]
print("Phase offset of %.3f, %.3f rad determined" % (phase_offset, np.angle(fixed_peak)))

phase_corr = np.multiply(freq_corr, np.exp(-phase_offset*1j))

exerpt = phase_corr[4 + start_idx + len(preamble_samps): 4 + start_idx + len(preamble_samps) + 32]
decode = OFDMtoSymbols(exerpt)
decode = fixWithReferenceSymbols(decode)
#print(decode)

print("Ref: %.3f, Sym 0: %.3f, Sym 30: %.3f, Sym - Offset: %.3f" % (np.angle(fixed_peak), np.angle(decode[0]), np.angle(decode[30]), (np.angle(decode[0]) - phase_offset) % (2*np.pi)))

f = phase_corr[4 + start_idx + len(preamble_samps): 4 + start_idx + len(preamble_samps) + 32 + 4*36]
plt.plot(np.imag(phase_corr))
plt.plot(np.real(phase_corr))
plt.show()


for i in range(30):
    exerpt = phase_corr[4 + start_idx + len(preamble_samps) + i*36: 4 + start_idx + len(preamble_samps) + 32 + i*36]
    decode = OFDMtoSymbols(exerpt)
    decode = fixWithReferenceSymbols(decode)
    plt.scatter(np.real(decode)[:], np.imag(decode)[:])

plt.xlim((-6, 6))
plt.ylim((-6, 6))
plt.show()