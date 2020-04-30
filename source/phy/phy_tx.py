from phy_common import *
import os
import errno
import collections
import scipy.signal as sig

def windowTransition(size, direction=True):
    div = float(size + 1)
    if direction:
        return 0.5 * (1 + np.cos(np.linspace(1, size, size)*np.pi/div))
    else:
        return 1 - 0.5 * (1 + np.cos(np.linspace(1, size, size)*np.pi/div))

pre_window = windowTransition(cp_samples, False)
post_window = windowTransition(cp_samples, True)

# Convert frequency-domain symbols to time-series samples
def symbolsToOFDM(syms):
    symbol_sets = []
    if len(syms) % data_sym_per_slice != 0:
        assert False

    for i in range(int(len(syms)/data_sym_per_slice)):
        # Set of data before reference insertion
        ofdm_set = syms[i*data_sym_per_slice:(i+1)*data_sym_per_slice]
        
        # Insert reference symbols
        ofdm_set = np.insert(ofdm_set, ref_sym_insertion, [ref_sym]*num_ref_sym)

        # Perform IFFT to get samples
        ofdm_samps = ifft(insertFFTDeadzone(ofdm_set))

        # Generate and append CP
        cp_pre = ofdm_samps[-cp_samples:]
        cp_post = ofdm_samps[:cp_samples]
        ofdm_samps = np.concatenate((cp_pre,ofdm_samps,cp_post))

        symbol_sets.append(ofdm_samps)

    all_samps = []
    all_samps.extend(symbol_sets[0])
    for x in symbol_sets[1:]:
        pre = x[:cp_samples]
        post = all_samps[-cp_samples:]
        trans = (pre * pre_window) + (post * post_window)

        all_samps[-cp_samples:] = trans
        all_samps.extend(x[cp_samples:])

    return np.multiply(all_samps, np.ceil(np.log2(fft_size))*float(fft_size)/used_fft_size)

# Generate set of symbols from bits
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

# Take a byte array and make a bit array
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

# Encode an OFDM frame from bytes
def encodeOFDMFrame(d):
    bits = stringToBits(d)
    syms = bitsToSymbols(bits)
    samps = symbolsToOFDM(syms)
    return samps

def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = sig.butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

# Generate real symbols from IQ data
def generateRealSamps(data_bytes):
    data_symbols = turboEncode(data_bytes, packet_turbo_rate, bits_per_sym)
    #print(data_symbols)
    iq = symbolsToOFDM(data_symbols)
    print("%d symbols for %d bytes" % (len(data_symbols), len(data_bytes)))
    iq = np.concatenate((preamble_samps, iq))
    print(len(iq))
    interpolate = np.repeat(iq, int(real_samp_rate/bandwidth)) # x12 to get to 48khz
    print(len(interpolate))

    b, a = butter_lowpass(2500, real_samp_rate, order=10)
    interpolate = sig.lfilter(b, a, interpolate)
    np.save("iq_sig.npy", interpolate)

    shift = shiftSamples(interpolate, 5000, sr=real_samp_rate)
    samps = np.real(shift)
    samps = np.multiply(samps, 0.005) # Scale down for hardware
    samps.astype('float32').tofile("real_sig.bin")
    return samps

def sendFloatSamps(samps):
    global write_file
    tx = np.array(samps, dtype=np.float32)
    btsr = tx.tobytes()
    try:
        write_file.write(btsr)
    except Exception as e:
        print(e)
        write_file = open(write_pipe, mode="w")

turboInit()

read_pipe = '/tmp/phy_tx_in'
write_pipe = '/tmp/phy_tx_out'

write_file = open(write_pipe, mode="w")

byte_queue = collections.deque([], maxlen=packet_user_bytes)
while True:
    print("Opening Read Pipe...")
    with open(read_pipe) as fifo:
        print("Read pipe opened")
        while True:
            data = fifo.read(packet_user_bytes)
            print(len(byte_queue))
            for x in data:
                byte_queue.append(x)
                if len(byte_queue) == byte_queue.maxlen:
                    print("Sent packet")
                    samps = generateRealSamps(byte_queue)
                    sendFloatSamps(samps)
                    byte_queue.clear()
            if len(data) == 0:
                print("Writer closed")
                byte_queue.clear()
                break

turboKill()