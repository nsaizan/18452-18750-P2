import numpy as np
import ctypes
import array

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

fft_size = 48
used_fft_size = 32
fft_deadzone = (fft_size - used_fft_size) / 2
bandwidth = 6e3
bits_per_sym = 2 #QPSK
subcarrier_width = bandwidth / fft_size
symbol_rate = subcarrier_width
sampling_rate = bandwidth
cp_samples = fft_size / 8
real_samp_rate = 48e3

num_ref_sym = 8
ref_sym_spacing = used_fft_size / num_ref_sym
ref_sym_insertion = [i * ref_sym_spacing - i for i in range(num_ref_sym)]
ref_sym_indexes = [i * ref_sym_spacing for i in range(num_ref_sym)]
ref_sym = 1 + 0j
data_sym_per_slice = used_fft_size - num_ref_sym


def fft(*args, **kwargs):
    return np.fft.fftshift(np.fft.fft(*args, **kwargs))

def ifft(*args, **kwargs):
    return np.fft.ifft(np.fft.ifftshift(*args, **kwargs))

def generateZCSequence(l):
    pn = list()
    nzc = l
    u = 1
    for n in range(nzc):
        pn.append(np.exp(-1j*np.pi*u*n*(n+np.mod(nzc, 2))/nzc))
    return pn

turbolib = ctypes.CDLL('/home/nsaizan/Documents/18452-18750-P2/source/phy/libsrslte_phy.so')
# make srslte_phy

def turboInit():
    turbolib.local_turbo_init()

def turboEncode(data, rate, mod):
    data_bits = len(data) * 8
    nof_re = int(np.round(data_bits / mod * rate))
    print(nof_re)

    if (mod == 2): srs_mod = 1
    if (mod == 4): srs_mod = 2

    bytestr = bytes(bytearray(data))
    in_ptr = ctypes.cast(bytestr, ctypes.POINTER(ctypes.c_uint8))

    temp_arr = [0] * int(nof_re) * 2
    syms = array.array('f', temp_arr)
    addr, size = syms.buffer_info()
    out_ptr = (ctypes.c_uint8 * size).from_address(addr)

    turbolib.local_turbo_encode(nof_re, srs_mod, len(data), in_ptr, out_ptr)

    real = np.take(syms, [i*2 for i in range(len(syms)/2)])
    imag = np.take(syms, [i*2+1 for i in range(len(syms)/2)])

    return np.array(real+imag*1j)

def turboDecode(syms, data_len, mod):
    nof_re = len(syms)
    average_mag = np.average(np.abs(syms))
    syms = np.multiply(syms, np.sqrt(2)/average_mag)

    if (mod == 2): srs_mod = 1
    if (mod == 4): srs_mod = 2

    temp_arr = [0] * data_len
    data = array.array('B', temp_arr)
    addr, size = data.buffer_info()
    out_ptr = (ctypes.c_uint8 * size).from_address(addr)

    bytestr = np.array(syms, dtype=np.complex64).tobytes()
    in_ptr = ctypes.cast(bytestr, ctypes.POINTER(ctypes.c_uint8))

    result = turbolib.local_turbo_decode(nof_re, srs_mod, data_len, out_ptr, in_ptr)

    return bytearray(data) if result == 1 else None

def turboKill():
    turbolib.local_turbo_end()

def insertFFTDeadzone(seq):
    return np.concatenate(([0+0j]*fft_deadzone,seq,[0+0j]*fft_deadzone))

def stripFFTDeadzone(seq):
    return seq[fft_deadzone:-fft_deadzone]

shortZC_raw = generateZCSequence(used_fft_size / 4)
shortZC = ifft(insertFFTDeadzone(fft(shortZC_raw * 4)))[:fft_size/4]
longZC_raw = generateZCSequence(used_fft_size)
longZC = ifft(insertFFTDeadzone(fft(longZC_raw)))
longZC_conj = np.conjugate(longZC)
shortZC_conj = np.conjugate(shortZC)
preamble_samps = np.multiply(np.concatenate((longZC,shortZC,shortZC,shortZC,shortZC)), float(fft_size)/used_fft_size)
longZC_thresh = used_fft_size * 0.65

packet_user_bytes = 63
packet_user_bits = packet_user_bytes*8
packet_turbo_rate = 2
packet_len_bits = packet_user_bits * packet_turbo_rate
packet_useful_syms = packet_len_bits / bits_per_sym
num_fft_slices =  int(np.ceil(float(packet_len_bits)/bits_per_sym/data_sym_per_slice))
packet_len_samps = int(len(preamble_samps) + num_fft_slices * (fft_size + cp_samples))

def shiftSamples(samps, freq, phase=0, sr=sampling_rate):
    return np.multiply(samps, np.exp(
        np.add(np.linspace(0,len(samps)-1,len(samps))*2*np.pi*1j*(float(freq)/sr),
        phase)))
