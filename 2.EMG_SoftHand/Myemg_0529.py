import argparse
import threading
import time
import pandas as pd
import pickle as pk
import matplotlib.pyplot  as plt
import serial
from filter import LowpassFilter1D
from processing import MeanShift1D, Normalize1D
from feature_extraction import *


# process signal of each channel
def process_signal1d(x, raw_fs=1000, low_fs=1):
    """
    @param x: signal of a single channel
    @param raw_fs: original sampling rate
    @param low_fs: low cutoff frequency
    @param high_fs: high cutoff frequency
    @param notch_fs: notch cutoff frequency
    @param Q: Q factor
    @param window_size: windows size for detrending
    @param step_size: step size for detrending
    @param target_fs: target sampling rate for resampling step
    """
    # mean-correct signal
    x_processed = MeanShift1D.apply(x)
    # rectify
    x_processed = abs(x_processed)
    # filtering noise
    x_processed = LowpassFilter1D.apply(x_processed, low_fs, order=4, fs=raw_fs)
    # plt.plot(x_processed,)
    # resample i uncomment this code
    # x_processed = Resample1D.apply(x_processed, raw_fs, target_fs)
    # rectify

    # normalize
    # x_processed = Normalize1D.apply(x_processed, norm_type='min_max')
    return x_processed

def process_signalnd(x, raw_fs=1000, low_fs=1, high_fs=120, notch_fs=60, Q=20, window_size=250, step_size=50, target_fs=512):
    """
    @param x: signal of a single channel
    @param raw_fs: original sampling rate
    @param low_fs: low cutoff frequency
    @param high_fs: high cutoff frequency
    @param notch_fs: notch cutoff frequency
    @param Q: Q factor
    @param window_size: windows size for detrending
    @param step_size: step size for detrending
    @param target_fs: target sampling rate for resampling step
    """
    num_channels = x.shape[1]
    x_processed = np.array([])
    for i in range(num_channels):
        # process each channel
        channel_processed = process_signal1d(x[:, i], raw_fs, low_fs)
        channel_processed = np.expand_dims(channel_processed, axis=1)
        if i == 0:
            x_processed = channel_processed
            continue
        x_processed = np.hstack((x_processed, channel_processed))
    return x_processed

# parameters for clean data
raw_fs = 1000
# target_fs = 512
low_fs = 10
# high_fs = 400
# notch_fs = 60
# Q = 20
# windows = 500 ##512 
# steps = 50
# segment parameters
seg_window_size = 500 #default:512
seg_step_size = 50 #default:32


# filename = ["Four_3.csv", "Four_4.csv"]
# for i in filename:
#     emg_raw = pd.read_csv(str(i)).values

# # clean raw signal
#     emg_processed = process_signalnd(emg_raw, raw_fs, low_fs, high_fs, notch_fs, Q, windows, steps, target_fs)
# # segment signal
# # signal_segments = SegmentND.apply(emg_processed, seg_window_size, seg_step_size)

#     plt.plot(emg_processed,)
#     plt.legend(['CH1', 'CH2', 'CH3', 'CH4'])
#     plt.show()


#     peak = MaxPeak.apply(emg_processed)
#     print(peak.mean())
    

import glob
for files in glob.glob("./data/0529_four-channel/test/Three/*.csv"):
    emg_raw = pd.read_csv(str(files)).values

    emg_processed = process_signalnd(emg_raw, raw_fs, low_fs)

    peak = MaxPeak.apply(emg_processed)
    mean = Mean.apply(emg_processed)
    print(peak.mean())
    print(mean.mean())
    