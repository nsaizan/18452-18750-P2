import numpy as np

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
real_samp_rate = 48e3

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
longZC_conj = np.conjugate(longZC)
shortZC_conj = np.conjugate(shortZC)
preamble_samps = longZC + 4 * shortZC
longZC_thresh = fft_size * 0.65

packet_user_bytes = 64
packet_user_bits = packet_user_bytes*8
packet_len_bits = packet_user_bits *2 #64 bytes doubled for redundancy
num_fft_slices =  int(np.ceil(float(packet_len_bits)/bits_per_sym/data_sym_per_slice))
packet_len_samps = int(len(preamble_samps) + num_fft_slices * (fft_size + cp_samples))

def shiftSamples(samps, freq, phase=0):
    return np.multiply(samps, np.exp(
        np.add(np.linspace(0,len(samps)-1,len(samps))*2*np.pi*1j*(float(freq)/sampling_rate),
        phase)))