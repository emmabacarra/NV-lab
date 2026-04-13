
import pyvisa
from spinapi import *

################################################################################
#
class SignalGenerator:
    # 
    # Define a SignalGenerator object :
    #   mostly done to hide the settings for the Anritsu MG3681A that should not change.
    #   Normally will make direct calls from the returned siggen object.
    #
    # Usage :
    # siggen = SignalGenerator()
    # siggen.write('FREQ 2.87 GHZ')
    #
    # For reference, here are all the commands that will be used :
    #
    # # Read commands
    # siggen.query('FREQ?')
    # siggen.query('OLVL?')
    # # Write commands
    # siggen.write('FREQ 328.22 MHz')
    # siggen.write('OLVL 25 mV')
    # # Change output units
    # siggen.write('OLDBM')  # dBm
    # siggen.write('OLDBU')  # dBuV
    # siggen.write('OLV')    # V
    # siggen.write('OLW')    # W
    # # Toggle RF output
    # siggen.write('LVL OFF')
    # siggen.write('LVL ON')
    # #
    def __init__(self):
        rm = pyvisa.ResourceManager()
        # rm.list_resources()

        self.siggen = rm.open_resource('ASRL4::INSTR')
        self.siggen.read_termination = '\r\n'
        # self.siggen.write_termination = '\n'
        self.siggen.baud_rate = 9600

        # initialize with power output at 0 dBm and off 
        self.siggen.write('OLDBM')
        self.siggen.write('OLVL 0')
        self.siggen.write('LVL OFF')

    def get_siggen(self):
        return self.siggen
    
    # The below functions aren't super useful since the Signal Generator object, 
    #    obtained from rm.open_resources, will already have these attributes 
    #    that can be called directly
    #
    def get_info(siggen):
        freq = siggen.query('FREQ?')
        lvl = siggen.query('OLVL?')
        return freq, lvl

    def set_freq_MHz(siggen, freq):
        fstr = 'FREQ ' + "%s" % freq  + 'MHZ'
        siggen.write(fstr)
        
    def set_level_mV(siggen, lvl):
        siggen.write(f'OLVL {lvl}mV')

        
        
################################################################################
#
class LockInAmplifier( ):
    # 
    # Define a LockInAmplifier object :
    #   mostly done to hide the settings for the SR-830 that should not change.
    #   Normally will make direct calls from the returned lockin object.
    #
    # Usage :
    # lockin = LockInAmplifier()
    # lockin.query('FREQ?')
    # lockin.query('OUTP? 3')

    # Full list of commands that we will need
    # # status of lockin
    # lockin.query('FREQ?')
    # lockin.query('SENS?')   # read sensitivity - index in 1 .. 26 according to table
    # lockin.query('OFLT?')   # read time constant - index in 1 .. 19 according to table
    # lockin.query('OFSL?')   # read low pass slope - index in 1 .. 3, 2 = 18dB
    # lockin.query('SYNC?')   # read sync filter - 0 off, 1 on

    # # Data transfer
    # lockin.query('OUTP? 3')  # retrieve R value
    # lockin.query('OUTP? 4')  # retrieve theta value

    # # Lockin settings
    # lockin.write('AGAN')     # auto gain
    # lockin.write('APHS')     # auto phase
    # lockin.write('SENS 6')   # set sensitivity - index in 1 .. 26 according to table
    # lockin.write('OFLT 6')   # set time constant - index in 1 .. 19 according to table
    # lockin.write('OFSL 2')   # set low pass slope - index in 0=6dB, 1=12dB, 2=18dB, 3=24dB
    # lockin.write('SYNC 1')   # set sync filter - 0=off, 1=on

    def __init__(self):
        rm = pyvisa.ResourceManager()
        # rm.list_resources()

        self.lockin = rm.open_resource('ASRL5::INSTR')
        self.lockin.read_termination = '\r'
        self.lockin.write_termination = '\r\n'
        self.lockin.baud_rate = 9600

        
    def get_lockin(self):
        return self.lockin



################################################################################
#
class PulseBlaster:

    def __init__(self, board_id = 0, board_freq = 500):
        self.board_id = board_id
        self.board_freq = board_freq

        print("Found %d boards in the system.\n" % pb_count_boards())
        pb_select_board(self.board_id)

        # Initialize communication with the board and catch any error
        if pb_init() != 0:
            print("Error initializing board: %s" % pb_get_error())
            input("Please press a key to continue.")
            exit(-1)

        pb_core_clock(self.board_freq)     # Set the internal clock frequency
        print("PulseBlaster is initialized and communication is open.\n")


    def trigger_pulse_program(self):
        # pb_reset()
        pb_start()


    def stop_pulse_program(self):
        pb_stop()


    def stop_trigger_and_close(self):
        pb_stop()
        pb_close()


    # Super useful for debugging - triggers the pulse program and then waits for keyboard input
    def trigger_and_wait(self):
        pb_reset() 
        pb_start()

        print("Pressing a key will stop pulsing\n");
        input("Please press a key to stop.")

        pb_stop()
        pb_close()
    
