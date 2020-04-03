import numpy as np
import matplotlib.pyplot as plt

fft_size = 32
bandwidth = 4e3
bits_per_sym = 4 # QPSK
subcarrier_width = bandwidth / fft_size
symbol_rate = subcarrier_width
sampling_rate = bandwidth
cp_samples = fft_size / 8

def symbolsToOFDM(syms):
    symbol_sets = []
    for i in range(int(len(syms)/fft_size)):
        ofdm_set = syms[i*fft_size:(i+1)*fft_size]
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
    if bits_per_sym == 4:
        #16QAM
        m = {0: 1/np.sqrt(10) + 1j/np.sqrt(10), 1: 1/np.sqrt(10) + 3j/np.sqrt(10),
            2: 3/np.sqrt(10) + 1j/np.sqrt(10), 3: 3/np.sqrt(10) + 3j/np.sqrt(10),
            4: 1/np.sqrt(10) - 1j/np.sqrt(10), 5: 1/np.sqrt(10) - 3j/np.sqrt(10),
            6: 3/np.sqrt(10) - 1j/np.sqrt(10), 7: 3/np.sqrt(10) - 3j/np.sqrt(10),
            8: -1/np.sqrt(10) + 1j/np.sqrt(10), 9: -1/np.sqrt(10) + 3j/np.sqrt(10),
            10: -3/np.sqrt(10) + 1j/np.sqrt(10), 11: -3/np.sqrt(10) + 3j/np.sqrt(10),
            12: -1/np.sqrt(10) - 1j/np.sqrt(10), 13: -1/np.sqrt(10) - 3j/np.sqrt(10),
            14: -3/np.sqrt(10) - 1j/np.sqrt(10), 15: -3/np.sqrt(10) - 3j/np.sqrt(10)}
    if bits_per_sym == 2:
        #QPSK
        m = {0: np.exp(np.pi*5/4*1j), 1: np.exp(np.pi*3/4*1j), 
            2: np.exp(np.pi*7/4*1j), 3: np.exp(np.pi*1/4*1j)}
    if bits_per_sym == 1:
        #BPSK
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

def encodeOFDMFrame(d):
    bits = stringToBits(d)
    syms = bitsToSymbols(bits)
    samps = symbolsToOFDM(syms)
    return samps

def OFDMtoSymbols(time_syms):
    symbols = np.fft.fft(time_syms)
    return symbols

def shiftOFDMFreq(syms, samp_shift):
    return np.multiply(syms, np.exp(np.linspace(0,fft_size-1,fft_size)*2*np.pi*1j*(float(samp_shift)/fft_size)))

data = "1234123412341234"
tx_iq = encodeOFDMFrame(data)
decode = OFDMtoSymbols(tx_iq[39:71])
decode = shiftOFDMFreq(decode, 1)
print(decode)

#data = decodeOFDMFrame(tx_iq)

#plt.plot(np.real(tx_iq))
#plt.plot(np.imag(tx_iq))
plt.scatter(np.real(decode)[:4], np.imag(decode)[:4])
plt.xlim((-1, 1))
plt.ylim((-1, 1))
plt.show()