from phy_common import *
import matplotlib.pyplot as plt
import os
import errno
import collections
import sys
import time

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

last_freq = None
def fixWithReferenceSymbols(syms):
    global last_freq
    refs = np.take(syms, ref_sym_indexes)
    #print("Pre: %r" % np.diff(np.unwrap(np.angle(refs))))
    rad_per_sym = np.average(np.diff(np.unwrap(np.angle(refs)))) / ref_sym_spacing
    if not last_freq: last_freq = rad_per_sym
    else: last_freq = last_freq * 0.5 + rad_per_sym * 0.5

    unshift = np.multiply(syms, np.exp(np.linspace(0,used_fft_size-1,used_fft_size)*1j*-last_freq))
    refs = np.take(unshift, ref_sym_indexes)
    #print("Post: %r" %np.diff(np.unwrap(np.angle(refs))))

    desired_ang = np.angle(ref_sym)
    cur_ang = np.angle(np.average(refs))
    return np.multiply(unshift, np.exp(1j*(desired_ang-cur_ang)))

def OFDMtoSymbols(time_syms):
    symbols = fft(time_syms)
    return symbols[fft_deadzone:-fft_deadzone]

def getDataSymbols(packet_samps):
    all_data_syms = []
    start_samp = len(preamble_samps)
    #print("%d %d" %(len(preamble_samps), len(packet_samps)))
    for i in range(num_fft_slices):
        start = start_samp + i*(fft_size+cp_samples) + cp_samples - 1 # Not quite perfect sampling
        end = start + fft_size
        fft_slice = packet_samps[start:end]
        syms = OFDMtoSymbols(fft_slice)
        fixed_syms = fixWithReferenceSymbols(syms)
        data_syms = np.delete(fixed_syms, ref_sym_indexes)

        #hit = False
        #for j in range(len(data_syms)):
        #    dif = (np.angle(data_syms[j]) + np.pi + np.pi/4.0) % (np.pi/2.0) 
        #    if dif < np.pi * .375 and dif > np.pi * .125:
        #        print("error %d %d" % (i, j))
        #        hit = True
        #if hit:
        #    print(fixed_syms)
        #    fixed_syms = fixWithReferenceSymbols(syms)
        
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
    freq_corr = shiftSamples(packet_samps, -freq_offset)
    #freq_corr = packet_samps
    # Find phase offset
    phase_offset = np.average(extractShortRefs(freq_corr, 0))
    #print(phase_offset)
    # Correct for phase offset
    phase_corr = np.multiply(freq_corr, np.exp(-phase_offset*1j))
    np.save("last_packet.npy", phase_corr)
    
    #plt.plot(np.real(phase_corr))
    #plt.plot(np.imag(phase_corr))
    #plt.show()
    # Get data symbols
    data_syms = getDataSymbols(phase_corr)
    return data_syms

def calcPER():
    count = 0
    for i, x in enumerate(lastn):
        if x < time.time() - 20:
            del lastn[i]
    
    per = 1 - (float(len(lastn)) / 40.0)
    print("PER (last 20 sec): %.3f" % per)

total_count = 0
good_count = 0
peaks = []
lastn = []
t = time.time()
tc = 0
def processSample(s):
    global rx_state, packet_samps_count, total_count, good_count, peaks, t, tc
    tc += 1

    if time.time() > t + 1.0:
        #print("Received samples: %d" % tc)
        tc = 0
        t = time.time()
        calcPER()

    if rx_state == MODE_WAIT:
        cazac_buffer.append(s)
        
        # Early return if not enough samps yet
        if (len(cazac_buffer) < len(longZC)): return

        peak = newLongSyncPeak()
        packet_found = testPeak(peak)
        #peaks.append(peak)

        if packet_found:
            #plt.plot(np.abs(peaks))
            #plt.show()
            #print(len(peaks))
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
            np.save("last_packet_raw.npy", packet_samps)
            syms = decodePacket()
            data_string = turboDecode(syms[:packet_useful_syms], packet_user_bytes, bits_per_sym)
            #data_string = dataSymsToBytes(syms)
            total_count += 1
            if (data_string == None): 
                #print("fail")
                #plt.scatter(np.real(syms), np.imag(syms))
                #plt.xlim((-6, 6))
                #plt.ylim((-6, 6))
                #plt.show()
                pass
            else:
                lastn.append(time.time())
                good_count += 1
                print("GOOD DECODE: %s" % data_string)
                write_file.write(data_string)
                write_file.flush()
            #plt.plot(np.real(packet_samps))
            #plt.plot(np.imag(packet_samps))

            #plt.scatter(np.real(syms[:packet_useful_syms]), np.imag(syms[:packet_useful_syms]))
            #plt.xlim((-2, 2))
            #plt.ylim((-2, 2))
            #plt.show()

            #print("PER: %.3f, PER 100: %.3f" % ((1 - good_count/float(total_count)), 1 - float(good_count/100.0)))
            sys.stdout.flush()
            packet_samps_count = 0
            rx_state = MODE_WAIT


#x = np.fromfile("samps.bin", dtype=np.complex64)
#for samp in x:
#    processSample(samp)

turboInit()

#while True:
#    processSample(1+1j)

read_pipe = '/tmp/phy_rx_in'
write_pipe = '/tmp/phy_rx_out'

try:
    os.mkfifo(read_pipe)
except OSError as oe:
    if oe.errno != errno.EEXIST:
        raise

print("pas")

write_file = open(write_pipe, mode="w")

print("pas")

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

turboKill()
