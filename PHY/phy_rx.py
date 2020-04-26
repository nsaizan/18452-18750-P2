from phy_common import *
import matplotlib.pyplot as plt
import os
import errno
import collections

cazac_buffer = collections.deque([], maxlen=len(longZC))
packet_samps = np.empty_like([0]*packet_len_samps, dtype=np.complex64)

MODE_WAIT = 0
MODE_COLLECT = 1
rx_state = MODE_WAIT
packet_samps_count = 0

# Test the cazac buffer against the known ZC sequence
def newLongSyncPeak():
    unit_vec_buf = np.exp(np.multiply(np.angle(cazac_buffer), 1j))
    return np.sum(np.multiply(unit_vec_buf, longZC_conj))

def testPeak(s):
    return np.abs(s) > longZC_thresh

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

    return np.unwrap(np.angle(refs))

def fixWithReferenceSymbols(syms):
    refs = np.take(syms, ref_sym_indexes)
    rad_per_sym = np.average(np.diff(np.unwrap(np.angle(refs)))) / ref_sym_spacing
    unshift = np.multiply(syms, np.exp(np.linspace(0,fft_size-1,fft_size)*1j*-rad_per_sym))

    desired_ang = np.angle(ref_sym)
    cur_ang = np.angle(np.average(np.take(unshift, ref_sym_indexes)))
    return np.multiply(unshift, np.exp(1j*(desired_ang-cur_ang)))

def OFDMtoSymbols(time_syms):
    symbols = np.fft.fft(time_syms)
    return symbols

def getDataSymbols(packet_samps):
    all_data_syms = []
    start_samp = len(preamble_samps)
    print("%d %d" %(len(preamble_samps), len(packet_samps)))
    for i in range(num_fft_slices):
        start = start_samp + i*(fft_size+cp_samples) + 3 # Not quite perfect sampling
        end = start + 32
        fft_slice = packet_samps[start:end]
        syms = OFDMtoSymbols(fft_slice)
        fixed_syms = fixWithReferenceSymbols(syms)
        data_syms = np.delete(fixed_syms, ref_sym_indexes)
        all_data_syms.extend(data_syms)
    return all_data_syms

def dataSymsToBytes(syms):
    bits = []
    angles = np.angle(syms)
    quads = np.ceil(np.divide(angles, np.pi/2))
    qpsk_map = {-1: 0, 0: 2, 1: 3, 2: 1}
    sym_vals = [qpsk_map[int(x)] for x in quads]
    for val in sym_vals:
        if val == 0: bits.extend([0,0])
        if val == 1: bits.extend([0,1])
        if val == 2: bits.extend([1,0])
        if val == 3: bits.extend([1,1])

    string = ""
    for i in range(len(bits)/8):
        val = 0
        place = 128
        for j in range(8):
            val += place * bits[i*8+j]
            place /= 2
        string += chr(val)

    return string

def decodePacket():
    # Get phases of short reference symbols
    r = extractShortRefs(packet_samps, 0)
    # Calculate a freqeuncy offset based on the differences between each phase
    freq_offset = np.average(np.diff(r)) / len(shortZC) / 2 / np.pi * bandwidth
    #print("Found freq offset %.3f" % freq_offset)
    # Correct for frequency offset
    #freq_corr = shiftSamples(packet_samps, -freq_offset)
    freq_corr = packet_samps
    # Find phase offset
    phase_offset = extractShortRefs(freq_corr, 0)[3]
    # Correct for phase offset
    phase_corr = np.multiply(freq_corr, np.exp(-phase_offset*1j))
    # Get data symbols
    data_syms = getDataSymbols(phase_corr)
    return data_syms

def processSample(s):
    global rx_state, packet_samps_count
    if rx_state == MODE_WAIT:
        cazac_buffer.append(s)
        
        # Early return if not enough samps yet
        if (len(cazac_buffer) < len(longZC)): return

        peak = newLongSyncPeak()
        packet_found = testPeak(peak)

        if packet_found:
            # Prepopulate with longZC samples
            np.place(packet_samps,
                [True]*len(cazac_buffer) + [False]*(len(packet_samps)-len(cazac_buffer)),
                cazac_buffer)
            packet_samps_count = len(cazac_buffer)

            cazac_buffer.clear()
            rx_state = MODE_COLLECT

    if rx_state == MODE_COLLECT:
        packet_samps[packet_samps_count] = s
        packet_samps_count += 1

        # Have complete packet, now process it
        if packet_samps_count >= len(packet_samps):
            syms = decodePacket()
            data_string = dataSymsToBytes(syms)
            if (data_string[:18] != "hello world its me"): 
                print("fail")
                plt.scatter(np.real(syms), np.imag(syms))
                plt.xlim((-6, 6))
                plt.ylim((-6, 6))
                plt.show()
            print(data_string)
            write_file.write(data_string)
            write_file.flush()
            packet_samps_count = 0
            rx_state = MODE_WAIT


#x = np.fromfile("samps.bin", dtype=np.complex64)
#for samp in x:
#    processSample(samp)

read_pipe = 'phy_rx_in'
write_pipe = 'phy_rx_out'

try:
    os.mkfifo(read_pipe)
except OSError as oe:
    if oe.errno != errno.EEXIST:
        raise

write_file = open(write_pipe, mode="w")

byte_queue = collections.deque([], maxlen=8)
while True:
    print("Opening Read Pipe...")
    with open(read_pipe) as fifo:
        print("Read pipe opened")
        while True:
            data = fifo.read(8)
            for x in data:
                byte_queue.append(x)
                if len(byte_queue) == byte_queue.maxlen:
                    processSample(np.frombuffer(bytearray(byte_queue), dtype=np.complex64)[0])
                    byte_queue.clear()
            if len(data) == 0:
                print("Writer closed")
                break