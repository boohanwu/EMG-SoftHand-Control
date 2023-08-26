import argparse
import threading
import time
import pickle as pk
import serial
from filter import BandpassFilter1D, NotchFilter1D
from processing import MeanShift1D, Detrend1D, Resample1D, Normalize1D
from feature_extraction import *


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
    # filtering noise
    x_processed = BandpassFilter1D.apply(x_processed, low_fs, high_fs, order=4, fs=raw_fs)
    x_processed = NotchFilter1D.apply(x_processed, notch_fs, Q=Q, fs=raw_fs)
    # detrend
    x_processed = Detrend1D.apply(x_processed, detrend_type='locreg', window_size=window_size, step_size=step_size) 
    # resample i uncomment this code
    # x_processed = Resample1D.apply(x_processed, raw_fs, target_fs)
    # rectify
    x_processed = abs(x_processed)
    # normalize
    x_processed = Normalize1D.apply(x_processed, norm_type='min_max')
    return x_processed

# process multi-channel signal
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
        channel_processed = process_signal1d(x[:, i], raw_fs, low_fs, high_fs, notch_fs, Q, window_size, step_size, target_fs)
        channel_processed = np.expand_dims(channel_processed, axis=1)
        if i == 0:
            x_processed = channel_processed
            continue
        x_processed = np.hstack((x_processed, channel_processed))
    return x_processed

# The class to connect the electrodes through serial
class SerialPort:
    def __init__(self, port='COM3', baud=115200, cls=None, pca=None, controller=None, num_channels=5, interval=1000, timeout=0.1):
        super(SerialPort, self).__init__()
        self.port = serial.Serial(port, baud)
        self.signal = None
        self.interval = interval
        self.cls = cls
        self.num_channels = num_channels
        self.timeout = timeout
        self.feature_window_size = 10   # Please modify as your setting
        self.concat = True   # Please change as your setting
        self.avg_pool = True # Please change as your setting
        self.pca = pca
        self.controller = controller

    def serial_open(self):
        if not self.port.isOpen():
            self.port.open()
            print("open")

    def serial_close(self):
        self.port.close()

    def serial_send(self):
        print('Send action...')
        time.sleep(self.timeout)
        if self.action == '0':
            print("Relax")
        elif self.action == '1':
            print("HandHold")
            # self.controller.thumb_move()
        elif self.action == '2':
            print("HandOpen")
        elif self.action == '3':
            print("HandRight")
            # self.controller.index_move()
        elif self.action == '4':
            print("HandLeft")
            # self.controller.middle_move()
        else:
            print("ouo")

    def serial_read(self):
        print('Receiving signal...')
        self.action = '0'

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
                kurt = Kurtosis.apply(signal_processed, self.feature_window_size).T.flatten()
                rms = RootMeanSquare.apply(signal_processed, self.feature_window_size).T.flatten()
                if self.concat:
                    feature = np.hstack([peak, mean, var, std, skew, kurt, rms])
                    feature = np.expand_dims(feature, axis=0)
                else:
                    feature = np.vstack([peak, mean, var, std, skew, kurt, rms])
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
                y_preds = self.cls.predict(feature)
                
                self.action = str(y_preds[0])
            # print(self.action)

class RobotController:      # // Define by myself
    def __init__(self):
        super(RobotController, self).__init__()
        self.port=serial.Serial('COM9', 115200)
        self.port.close()
        if not self.port.isOpen():
            self.port.open()
    def init_pos(self):
        """Initialize positions"""
        self.port.write(b'90,90,90,90,90,5000')
        print("Initial position")
    def thumb_move(self):
        self.port.write(b'10,90,90,90,90,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,5000')
        print("Thumb moving")
    def index_move(self):
        self.port.write(b'90,10,90,90,90,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,5000')
        print("Index moving")
    def middle_move(self):
        self.port.write(b'90,90,10,90,90,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,5000')
        print("Middle moving")
    def ring_move(self):
        self.port.write(b'90,90,90,10,90,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,5000')
        print("Ring moving")
    def pinky_move(self):
        self.port.write(b'90,90,90,90,10,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,5000')
        print("Pinky moving")
    def pinky_move(self):
        self.port.write(b'90,90,90,90,10,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,3000')
        print("Pinky moving")
        
    def hold(self):
        self.port.write(b'10,10,10,10,10,5000')
        time.sleep(8)
        self.port.write(b'90,90,90,90,90,3000')
        
    def right_move(self):
        self.port.write(b'10,90,90,90,90,5000')
        time.sleep(5)
        self.port.write(b'10,10,90,90,90,5000')
        time.sleep(5)
        self.port.write(b'10,10,10,90,90,5000')
        time.sleep(5)
        self.port.write(b'10,10,10,10,90,5000')
        time.sleep(5)
        self.port.write(b'10,10,10,10,10,5000')
        time.sleep(5)
        self.port.write(b'90,90,90,90,90,3000')
        print("right moving")
        
    def left_move(self):
        self.port.write(b'90,90,90,90,10,5000')
        time.sleep(5)
        self.port.write(b'90,90,90,10,10,5000')
        time.sleep(5)
        self.port.write(b'90,90,10,10,10,5000')
        time.sleep(5)
        self.port.write(b'90,10,10,10,10,5000')
        time.sleep(5)
        self.port.write(b'10,10,10,10,10,5000')
        time.sleep(5)
        self.port.write(b'90,90,90,90,90,3000')
        print("left moving")

if __name__ == '__main__':
    # Set command line arguments
    parser = argparse.ArgumentParser(description='Real-time robot-arm controlling')
    parser.add_argument('--port', type=str, default='COM3', help='COM port for arduino')
    parser.add_argument('--baud_rate', type=int, default=115200, help='Baud rate for arduino')
    parser.add_argument('--axport', type=str, default='COM9', help='COM port for my soft robot hand')
    parser.add_argument('--axbaud', type=int, default=115200, help='Baud rate for my soft robot hand')
    # parser.add_argument('--num-motors', type=int, default=4, help='Number of motors')
    parser.add_argument('--channels', type=int, default=5, help='The number of channels')
    parser.add_argument('--segment', type=int, default=1000, help='Segmentation interval')
    parser.add_argument('--timeout', type=float, default=0.5, help='Time out')
    args = parser.parse_args()
    # define reboot
    controller = RobotController()
    controller.init_pos()
    # define classifier
    cls = pk.load(open('nn.pkl', 'rb'))
    # define pre-processing pca
    # pca = pk.load(open('pca.pkl', 'rb'))
    # Setup serial line
    mserial = SerialPort(args.port, args.baud_rate, cls=cls, pca=None, controller=controller, num_channels=args.channels, interval=args.segment, timeout=args.timeout)   # pca set as None
    t1 = threading.Thread(target=mserial.serial_read)
    t1.start()
    try:
        while True:
            mserial.serial_send()
    except KeyboardInterrupt:
        print('Press Ctrl-C to terminate while statement')
    mserial.serial_close()
    # controller.disconnect()