import numpy as np
import matplotlib.pyplot as plt

fft_size = 32
bandwidth = 8e3
bits_per_sym = 2 # QPSK
subcarrier_width = bandwidth / fft_size
symbol_rate = subcarrier_width
sampling_rate = bandwidth
cp_samples = fft_size / 8

def filterCP(syms):
    filtered_syms = []
    for sym in syms:
        post_window = 0.5*(1+ \
                np.cos(np.pi*np.array(range(1, cp_samples+1))/cp_samples))
        pre_window = post_window[::-1]
        pre_sym = np.multiply(pre_window, sym[:cp_samples])
        post_sym = np.multiply(post_window, sym[-cp_samples:])
        filtered_syms.append(
            np.concatenate((pre_sym,sym[cp_samples:-cp_samples],post_sym)))

    combined_samples = []
    combined_samples.extend(filtered_syms[0])
    for i in range(1,len(filtered_syms)):
        sym = filtered_syms[i]
        c = np.add(combined_samples[-cp_samples:], sym[:cp_samples])/2
        combined_samples[-cp_samples:] = c
        combined_samples.extend(sym[cp_samples:])

    return combined_samples

def symbolsToOFDM(syms):
    symbol_sets = []
    for i in range(int(len(syms)/fft_size)):
        ofdm_set = syms[i*fft_size:(i+1)*fft_size]
        ofdm_samps = np.fft.ifft(ofdm_set)
        cp_pre = ofdm_samps[-cp_samples:]
        cp_post = ofdm_samps[:cp_samples]
        ofdm_samps = np.concatenate((cp_pre,ofdm_samps,cp_post))
        symbol_sets.append(ofdm_samps)
    return symbol_sets

def bitsToSymbols(b):
    syms = []

    m = dict()
    if bits_per_sym == 2:
        #QPSK
        m = {0: np.exp(np.pi*5/4*1j), 1: np.exp(np.pi*3/4*1j), 2: np.exp(np.pi*7/4*1j), 3: np.exp(np.pi*1/4*1j)}
    if bits_per_sym == 1:
        #QPSK
        m = {0: np.exp(np.pi*5/4*1j), 1: np.exp(np.pi*1/4*1j)}

    for i in range(len(b)/bits_per_sym):
        val = 0
        place = bits_per_sym
        for bit in range(bits_per_sym):
            val += b[i+bit] * place
            place /= 2
        sym = m[val]
        syms.append(sym)

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

data = "1234123412341234"
bits = stringToBits(data)
syms = bitsToSymbols(bits)
ofdm = symbolsToOFDM(syms)
samps = filterCP(ofdm)

plt.plot(np.real(samps))
plt.plot(np.imag(samps))
plt.show()