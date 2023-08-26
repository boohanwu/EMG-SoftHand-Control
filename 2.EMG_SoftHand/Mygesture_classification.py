import argparse
import threading
import time
import pickle as pk
import serial
from filter import BandpassFilter1D, NotchFilter1D,LowpassFilter1D
from processing import MeanShift1D, Detrend1D, Resample1D, Normalize1D
from feature_extraction import *
import socket

# s=socket.socket()
# s.bind(('127.0.0.1',36000))
# s.listen(10)
# c, addr = s.accept()

# def def_socket_thread(str_content):
#     # global loop
#     # loop = asyncio.get_event_loop()
#     try:
#         c.send(str_content.encode('ascii'))
#         print('send finish')
#     except IOError as e:
#         print(e.strerror)
#     print('start socket thread!!!')
    

# process signal of each channel
def process_signal1d(x, raw_fs=1000, low_fs=1, high_fs=120, notch_fs=60, Q=20, window_size=250, step_size=50, target_fs=512):
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
    # x_processed = BandpassFilter1D.apply(x_processed, low_fs, high_fs, order=4, fs=raw_fs)
    # x_processed = NotchFilter1D.apply(x_processed, notch_fs, Q=Q, fs=raw_fs)
    # detrend
    # x_processed = Detrend1D.apply(x_processed, detrend_type='locreg', window_size=window_size, step_size=step_size) 
    # resample i uncomment this code
    # x_processed = Resample1D.apply(x_processed, raw_fs, target_fs)

    # normalize
    # x_processed = Normalize1D.apply(x_processed, norm_type='min_max')
    return x_processed

# process multi-channel signal
def process_signalnd(x, raw_fs=1000, low_fs=1, high_fs=120, notch_fs=60, Q=20, window_size=1000, step_size=50, target_fs=512):
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
        channel_processed = process_signal1d(x[:, i], raw_fs, low_fs, high_fs, notch_fs, Q, window_size, step_size, target_fs)
        channel_processed = np.expand_dims(channel_processed, axis=1)
        if i == 0:
            x_processed = channel_processed
            continue
        x_processed = np.hstack((x_processed, channel_processed))
    return x_processed

# The class to connect the electrodes through serial
class SerialPort:
    def __init__(self, port='COM3', baud=115200, cls=None, pca=None, num_channels=4, interval=1000, timeout=0.1):
        super(SerialPort, self).__init__()
        self.port = serial.Serial(port, baud)
        self.signal = None
        self.interval = interval
        self.cls = cls
        self.num_channels = num_channels
        self.timeout = timeout
        self.feature_window_size = 1000   # Please modify as your setting
        self.concat = True   # Please change as your setting
        self.avg_pool = False # Please change as your setting
        self.pca = pca


    def serial_open(self):
        if not self.port.isOpen():
            self.port.open()
            print("open")

    def serial_close(self):
        self.port.close()

    def serial_send(self):
        print('Send action...')
        time.sleep(self.timeout)
        if self.action == '8':
            print("Relax")
            # def_socket_thread('1')
        elif self.action == '0':
            print("Hold")
            # def_socket_thread('2')
        elif self.action == '1':
            print("Open")
            # def_socket_thread('3')
        elif self.action == '2':
            print("Two")
            # def_socket_thread('4')
        elif self.action == '3':
            print("Three")
            # def_socket_thread('5')
        elif self.action == '4':
            print("Four")
            # def_socket_thread('6')
        elif self.action == '5':
            print("Six")
            # def_socket_thread('7')
        elif self.action == '6':
            print("Seven")
            # def_socket_thread('8')
        else:
            print("Relax")



    def serial_read(self):
        print('Receiving signal...')
        self.action = '8'
        # def_socket_thread('8')
        # time.sleep(1)

        while True:
            values = []

            # read signal from serial
            for i in range(self.interval):
                string = self.port.readline().decode('utf-8').rstrip()  # Read and decode a byte string
                values.extend([float(value) for value in string.split(',')])
            
            # print(len(values))
            
            if len(values) == self.interval * self.num_channels:    #  
                # reshape signal
                signal = np.reshape(np.array(values), (self.interval, self.num_channels), order='C')
                # process signal
                # please change parameters as your settings
                signal_processed = process_signalnd(signal, raw_fs=1000, low_fs=10, high_fs=400, notch_fs=60, Q=20, window_size=1000, step_size=50, target_fs=512)   # default window_size:250
                
                # extract, transpose and flatten feature vectors
                # change your feature as your setting
                peak = MaxPeak.apply(signal_processed, self.feature_window_size).T.flatten()
                mean = Mean.apply(signal_processed, self.feature_window_size).T.flatten()
                var = Variance.apply(signal_processed, self.feature_window_size).T.flatten()
                std = StandardDeviation.apply(signal_processed, self.feature_window_size).T.flatten()
                skew = Skewness.apply(signal_processed, self.feature_window_size).T.flatten()
                # kurt = Kurtosis.apply(signal_processed, self.feature_window_size).T.flatten()
                # rms = RootMeanSquare.apply(signal_processed, self.feature_window_size).T.flatten()
                if self.concat:
                    feature = np.hstack([peak, mean, var, std, skew])
                    feature = np.expand_dims(feature, axis=0)
                else:
                    feature = np.vstack([peak, mean, var, std, skew])
                    if self.avg_pool:
                        # average pooling
                        feature = feature.mean(axis=0)
                    else:
                        # max pooling
                        feature = feature.max(axis=0)
                    feature = np.expand_dims(feature, axis=0)
                # print(feature)
                if self.pca:
                    feature = self.pca.transform(feature)
                
                T1 = 9
                T2 = 8
                if peak.mean() <= T1 and mean.mean() <= T2:
                    # rest gesture
                    # change the label of this segment into -1
                    self.action = '8'
                else:
                    y_preds = self.cls.predict(feature)
                    self.action = str(y_preds[0])
                
            # print(self.action)


if __name__ == '__main__':
    # Set command line arguments
    parser = argparse.ArgumentParser(description='Real-time robot-arm controlling')
    parser.add_argument('--port', type=str, default='COM3', help='COM port for arduino')
    parser.add_argument('--baud_rate', type=int, default=115200, help='Baud rate for arduino')
    parser.add_argument('--axport', type=str, default='COM9', help='COM port for my soft robot hand')
    parser.add_argument('--axbaud', type=int, default=115200, help='Baud rate for my soft robot hand')
    # parser.add_argument('--num-motors', type=int, default=4, help='Number of motors')
    parser.add_argument('--channels', type=int, default=4, help='The number of channels')
    parser.add_argument('--segment', type=int, default=1000, help='Segmentation interval')
    parser.add_argument('--timeout', type=float, default=1, help='Time out')
    args = parser.parse_args()
    # define classifier
    cls = pk.load(open('nn.pkl', 'rb'))
    # define pre-processing pca
    # pca = pk.load(open('pca.pkl', 'rb'))
    # Setup serial line
    mserial = SerialPort(args.port, args.baud_rate, cls=cls, pca=None, num_channels=args.channels, interval=args.segment, timeout=args.timeout)   # pca set as None
    t1 = threading.Thread(target=mserial.serial_read)
    t1.start()
    try:
        while True:
            mserial.serial_send()
    except KeyboardInterrupt:
        print('Press Ctrl-C to terminate while statement')
    mserial.serial_close()
    # controller.disconnect()