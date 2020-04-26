from phy_common import *
import os
import errno
import collections

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
        ofdm_samps = np.fft.ifft(ofdm_set)

        # Generate and append CP
        cp_pre = ofdm_samps[-cp_samples:]
        ofdm_samps = np.concatenate((cp_pre,ofdm_samps))

        symbol_sets.append(ofdm_samps)

    all_samps = []
    for x in symbol_sets:
        all_samps.extend(x)

    return all_samps

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

# Generate real symbols from IQ data
def generateRealSamps(data_bytes):
    iq = encodeOFDMFrame(data_bytes)
    interpolate = np.repeat(iq, int(real_samp_rate/bandwidth)) # x12 to get to 48khz
    shift = shiftSamples(interpolate, 8000)
    samps = np.real(shift)
    return samps

def sendFloatSamps(samps):
    tx = np.array(samps, dtype=np.float32)
    btsr = tx.tobytes()
    write_file.write(btsr)

read_pipe = 'phy_tx_in'
write_pipe = 'phy_tx_out'

write_file = open(write_pipe, mode="w")

byte_queue = collections.deque([], maxlen=packet_user_bytes)
while True:
    print("Opening Read Pipe...")
    with open(read_pipe) as fifo:
        print("Read pipe opened")
        while True:
            data = fifo.read(1)
            for x in data:
                byte_queue.append(x)
                if len(byte_queue) == byte_queue.maxlen:
                    samps = generateRealSamps(byte_queue)
                    sendFloatSamps(samps)
                    byte_queue.clear()
            if len(data) == 0:
                print("Writer closed")
                byte_queue.clear()
                break